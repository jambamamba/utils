#include "debug_logger.h"
#include "file_utils.h"

#include <filesystem>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h> 
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>

LOG_CATEGORY(FILE_UTIL, "FILE_UTIL")

void FileUtils::getFilesFromDirectory(const std::string& directory, std::vector<std::string>& files)
{
    DIR *d;
    struct dirent *dir;
    LOG(DEBUG, FILE_UTIL, "directory : %s", directory.c_str());

    files.clear();

    d = opendir(directory.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            int len = strlen(dir->d_name);
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
                std::stringstream ss_job_dict_fullpath;
                ss_job_dict_fullpath << directory;
                ss_job_dict_fullpath << "/";
                ss_job_dict_fullpath << dir->d_name;          
                files.emplace_back(ss_job_dict_fullpath.str());
            }   
        }
        closedir(d);
    }
}

std::string FileUtils::getLastModifiedTime(const std::string& dictionaryName)
{
    struct stat st;
    stat(dictionaryName.c_str(), &st);
    struct timespec mtime = st.st_mtim;

    struct tm t;

    char time[256];
    strftime(time, 100, "%D %T", gmtime(&mtime.tv_sec));
    return time;
}

int FileUtils::getFileSize(const std::string& dictionary_name)
{
    struct stat st;
    stat(dictionary_name.c_str(), &st);
    size_t size = st.st_size;

    return (int)size;
}

std::string FileUtils::getBaseName(const std::string& fileFullPath) {
    return std::filesystem::path(fileFullPath).filename();
}


std::string FileUtils::getDirName(const std::string& fileFullPath) {
    return std::filesystem::path(fileFullPath).parent_path();
}

void FileUtils::makeDirectory(const std::string &directory)
{
    struct stat st = {0};
    if (stat(directory.c_str(), &st) == -1) {
        std::filesystem::create_directories(directory);
        mkdir(directory.c_str(), 0700);
    }
    // std::istringstream f(directory.c_str());
    // std::string s;    
    // while (std::getline(f, s, '/')) {
    //     s = "";
    // }
}

bool FileUtils::fileExists (const char *filename) 
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

bool FileUtils::folderExists (const char *filename) 
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

void FileUtils::deleteFile(const char *filepath)
{
    unlink(filepath);
}

void FileUtils::deleteAllFiles(const char *dir_path)
{
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) 
        std::filesystem::remove_all(entry.path());
}

std::string FileUtils::readFile(const char *path)
{
    std::string data;
    FILE *fp = fopen(path, "rb");
    char buffer[1024];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        int len = fread(buffer, 1, sizeof(buffer), fp);
        if(len <= 0){
            break;
        }
        data.append(buffer);
    }
    fclose(fp);
    return data;
}

void FileUtils::copyFile(const char *src, const char *dst)
{
    std::error_code ec;
    std::filesystem::copy_file(src, dst, 
        std::filesystem::copy_options::none,
        ec);
}

bool FileUtils::createFile (const char *filename)
{
    std::ofstream my_file(filename);

    return fileExists(filename);

}