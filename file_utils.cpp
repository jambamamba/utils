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

extern "C" {
#if defined(WIN32) || defined(MSYS)
char *__progname;
#endif
void setProgramName(const char* path_)
{
#if defined(WIN32) || defined(MSYS)
  auto tokens = FileUtils::splitString(path_, '\\');
  std::string progname = tokens.at(tokens.size() - 1);
  tokens = FileUtils::splitString(progname.c_str(), '.');
  __progname = strdup(tokens.at(0).c_str());
#endif
}
} //extern "C"

std::vector<std::string> FileUtils::splitString(const std::string& s, char seperator)
{
   std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}

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

std::string FileUtils::getLastModifiedTime(const std::string& file_name)
{
    struct stat st;
    char time[256] = {0};
    stat(file_name.c_str(), &st);
#ifdef WIN32
    strftime(time, 100, "%D %T", gmtime(&st.st_mtime));
#else
    strftime(time, 100, "%D %T", gmtime(&st.st_mtim.tv_sec));
#endif
return time;
}

int FileUtils::getFileSize(const std::string& file_name)
{
    struct stat st;
    stat(file_name.c_str(), &st);
    size_t size = st.st_size;

    return (int)size;
}

std::string FileUtils::getBaseName(const std::string& fileFullPath) {
#if defined(WIN32) || defined(MSYS)
    LOG(FATAL, FILE_UTIL, "not implemented");
    return "";//not implemented
#else
    return std::filesystem::path(fileFullPath).filename();
#endif
}

std::string FileUtils::getDirName(const std::string& fileFullPath) {
#if defined(WIN32) || defined(MSYS)
    LOG(FATAL, FILE_UTIL, "not implemented");
    return "";//not implemented
#else
    return std::filesystem::path(fileFullPath).parent_path();
#endif
}

extern "C" {
void makeDirectoryNonRecursive(const char* directory)
{
    mkdir(directory
#if !defined(WIN32) && !defined(MSYS)
    , 0700
#endif
    );
}
} //extern "C"

void FileUtils::makeDirectory(const std::string &directory)
{
    struct stat st = {0};
    if (stat(directory.c_str(), &st) == -1) {
#if defined(WIN32) || defined(MSYS)
        LOG(FATAL, FILE_UTIL, "not implemented");
#else
        std::filesystem::create_directories(directory);
#endif        
        makeDirectoryNonRecursive(directory.c_str());
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
#if defined(WIN32) || defined(MSYS)
    LOG(FATAL, FILE_UTIL, "not implemented");
#else
    for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
        std::filesystem::remove_all(entry.path());
    }
#endif        
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
#if defined(WIN32) || defined(MSYS)
    LOG(FATAL, FILE_UTIL, "not implemented");
#else
    std::error_code ec;
    std::filesystem::copy_file(src, dst, 
        std::filesystem::copy_options::none,
        ec);
#endif
}

bool FileUtils::createFile (const char *filename)
{
    std::ofstream my_file(filename);

    return fileExists(filename);

}