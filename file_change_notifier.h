#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <thread>

class FileChangeNotifier
{
public: 
    FileChangeNotifier();
    ~FileChangeNotifier();
    enum FileChangeNotifierEvent_E{
        DirectoryCreated,
        DirectoryDeleted,
        FileCreated,
        FileDeleted
    };
    bool RegisterINotifyCallback(const std::string &filepath, std::function<void(FileChangeNotifierEvent_E event, const std::string &)> cb);
    void UnregisterINotifyCallback(const std::string &filepath);

protected:
    struct Context {
        int _fd;
        int _watch;
        std::string _directory;
        std::function<void(FileChangeNotifierEvent_E, const std::string &)> _cb;
        Context(int fd, int watch, const std::string& directory, std::function<void(FileChangeNotifierEvent_E, const std::string &)> cb)
        : _fd(fd), _watch(watch), _directory(directory), _cb(cb)
        {}
    };
    std::map<std::string /*filepath*/, Context> _notify_map;
    std::mutex _mutex;
    bool _killed = false;
    std::unique_ptr<std::thread>_thread;

    void WatchFiles() const;
    void CheckMasks(const char *buf, int length, const FileChangeNotifier::Context &ctx) const;
    void CleanupNotifier(Context &ctx);
};