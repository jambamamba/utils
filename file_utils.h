#pragma once

#include <string>
#include <string.h>
#include <vector>

class FileUtils
{
public:
    static void getFilesFromDirectory(const std::string& directory, std::vector<std::string>& files);
    static std::string getLastModifiedTime(const std::string& file);
    static int getFileSize(const std::string& file);
    static std::string getBaseName(const std::string& fileFullPath);
    static std::string getDirName(const std::string& fileFullPath);
    static void makeDirectory(const std::string& directory);
    static bool fileExists (const char *filename);
    static bool folderExists (const char *filename);
    static void deleteAllFiles(const char *dir_path);
    static void deleteFile(const char *filepath);
    static std::string readFile(const char *path);
    static void copyFile(const char *src, const char *dst);
    static bool createFile (const char *filename);
};

