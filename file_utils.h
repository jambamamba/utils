#pragma once
#ifndef __cplusplus
    void setProgramName(const char* path_);
    void makeDirectoryNonRecursive(const char* directory);
#endif //__cplusplus

#ifdef __cplusplus
#if 0
#endif

#include <string>
#include <string.h>
#include <vector>

#include "debug_logger.h"

extern "C" void setProgramName(const char* path_);
namespace FileUtils
{
    std::vector<std::string> splitString(const std::string& s, char seperator);
    void getFilesFromDirectory(const std::string& directory, std::vector<std::string>& files);
    std::string getLastModifiedTime(const std::string& file);
    int getFileSize(const std::string& file);
    std::string getBaseName(const std::string& fileFullPath);
    std::string getDirName(const std::string& fileFullPath);
    void makeDirectory(const std::string& directory);
    bool fileExists (const std::string &filename);
    bool fileExists (const char *filename);
    bool folderExists (const std::string &dirname);
    bool folderExists (const char *dirname);
    void deleteAllFiles(const char *dir_path);
    void deleteFile(const char *filepath);
    std::string readFile(const char *path);
    void copyFile(const char *src, const char *dst);
    bool createFile (const char *filename);
    std::string getProgramName();
    std::string getProgramPath();
    std::string getProgramPathName();
    std::string &&toLinuxPathSeparators(std::string &&path);
}

#ifdef __cplusplus
#endif//0
#endif //__cplusplus

#define PROGRAM_INIT(path, workdir) \
    setProgramName(path); \
    debuglogger_set_log_dir(workdir);
    
