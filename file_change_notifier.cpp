#include <debug_logger.h>
#include <errno.h>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
#ifndef MINGW
#include <poll.h>
#include <sys/inotify.h>
#endif


#include "file_change_notifier.h"
#include <file_utils.h>


#ifdef MINGW
struct inotify_event {}; //osm todo
#endif
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

LOG_CATEGORY(INOTIFY, "INOTIFY")

FileChangeNotifier::FileChangeNotifier()
{
    std::scoped_lock lock(_mutex);
    if(_notify_map.size() == 0){
        _thread = std::make_unique<std::thread>(std::thread([this](){
            while(!_killed){
                // std::scoped_lock lock(_mutex);
                WatchFiles();
            }
        }));
    }
}

FileChangeNotifier::~FileChangeNotifier()
{
    std::scoped_lock lock(_mutex);
    for(auto &[directory, ctx] : _notify_map){
        CleanupNotifier(ctx);
    }
    _notify_map.clear();
    _killed = true;
    if(_notify_map.size() == 0){
        _thread->join();
    }
}

void FileChangeNotifier::CleanupNotifier(FileChangeNotifier::Context &ctx)
{
    inotify_rm_watch(ctx._fd, ctx._watch);
    close(ctx._fd);
}

void FileChangeNotifier::UnregisterINotifyCallback(const std::string &directory)
{
    std::scoped_lock lock(_mutex);
    
    auto it = _notify_map.find(directory);
    if(it == _notify_map.end()){
        return;
    }
    CleanupNotifier(it->second);
    _notify_map.erase(directory);
}

bool FileChangeNotifier::RegisterINotifyCallback(const std::string &directory, 
    std::function<void(FileChangeNotifierEvent_E, const std::string &)> cb)
{
    if(_notify_map.find(directory) != _notify_map.end()) {
        UnregisterINotifyCallback(directory);
    }

    std::scoped_lock lock(_mutex);

    FileUtils::makeDirectory(directory);

    int fd = inotify_init1(IN_NONBLOCK);
    if (fd < 0){
        LOG(FATAL, INOTIFY, "inotify_init error [%i] %s\n", errno, strerror(errno));
        return false;
    }

    int wd = inotify_add_watch(fd, directory.c_str(), IN_CREATE | IN_DELETE);
    if (wd <= 0){
        LOG(FATAL, INOTIFY, "inotify_add_watch error [%i] %s\n", errno, strerror(errno));
        return false;
    }
    _notify_map.insert(std::pair<const std::string, 
        FileChangeNotifier::Context>(directory, FileChangeNotifier::Context(fd, wd, directory, cb)));

    return true;
}

void FileChangeNotifier::CheckMasks(const char *buf, int length, const FileChangeNotifier::Context &ctx) const
{
    int i = 0;
    while (i < length){
        struct inotify_event *event = (struct inotify_event *)&buf[i];
        if (event->len){
            std::string filepath = ctx._directory + "/" + event->name;
            if (event->mask & IN_CREATE){
                if (event->mask & IN_ISDIR){
                    LOG(DEBUG, INOTIFY, "New directory %s created.\n", filepath.c_str());
                    ctx._cb(FileChangeNotifier::FileChangeNotifierEvent_E::DirectoryCreated, filepath);
                }
                else{
                    LOG(DEBUG, INOTIFY, "New file %s created.\n", filepath.c_str());
                    ctx._cb(FileChangeNotifier::FileChangeNotifierEvent_E::FileCreated, filepath);
                }
            }
            else if (event->mask & IN_DELETE){
                if (event->mask & IN_ISDIR){
                    LOG(DEBUG, INOTIFY, "Directory %s deleted.\n", filepath.c_str());
                    ctx._cb(FileChangeNotifier::FileChangeNotifierEvent_E::DirectoryDeleted, filepath);
                }
                else{
                    LOG(DEBUG, INOTIFY, "File %s deleted.\n", filepath.c_str());
                    ctx._cb(FileChangeNotifier::FileChangeNotifierEvent_E::FileDeleted, filepath);
                }
            }
        }
        i += EVENT_SIZE + event->len;
    }
}

void FileChangeNotifier::WatchFiles() const
{
#ifdef MINGW
#else
    std::unique_ptr<struct pollfd[]> pfds;
    int num_fds = _notify_map.size();
    pfds = std::make_unique<struct pollfd[]>(num_fds);
    
    std::vector<FileChangeNotifier::Context> contexts;
    int j = 0;
    for (auto &[directory, ctx]: _notify_map) {
        pfds[j].fd = ctx._fd;
        pfds[j].events = POLLIN;
        pfds[j].revents = 0;
        contexts.push_back(ctx);
    }

    const int TWO_SECONDS = 1000 * 2;
    int ready = poll(pfds.get(), num_fds, TWO_SECONDS);
    if(ready == -1){
        LOG(FATAL, INOTIFY, "poll error [%i] %s\n", errno, strerror(errno));
        return;
    }
    for (int j = 0; j < num_fds; j++) {
        if (pfds[j].revents != 0) {
            if (pfds[j].revents & POLLIN) {
                char buf[EVENT_BUF_LEN] = {0};
                int length = read(pfds[j].fd, buf, sizeof(buf));
                if (length == -1){
                    LOG(FATAL, INOTIFY, "poll read error [%i] %s\n", errno, strerror(errno));
                    return;
                }

                CheckMasks(buf, length, contexts.at(j));
            } else {                /* POLLERR | POLLHUP */
                // LOG(DEBUG, INOTIFY, "closing fd %d\n", pfds[j].fd);
                // if (close(pfds[j].fd) == -1){
                //     LOG(FATAL, INOTIFY, "poll close error [%i] %s\n", errno, strerror(errno));
                // }
            }
        }
    }
#endif//not MINGW
}