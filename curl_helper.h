#pragma once

#include <atomic>
#include <cJSON.h>
#include <curl/curl.h>
#include <functional>
#include <string>
#include <map>


class CurlHelper {
    struct CurlContext {
        CurlContext(std::function<bool (long curl_result, long http_status_code, ssize_t bytes_written, ssize_t content_length, const std::string &errmsg)> 
            progressfn = nullptr);
        ~CurlContext();
        bool loadFromJson(const cJSON* json);
        long httpStatusCode() const;
        std::string errorToString() const;


        CURLcode _code = (CURLcode) CURLM_UNKNOWN_OPTION;
        CURL *_handle = nullptr;
        char *_memory = nullptr;
        size_t _size = 0;
        FILE *_filep = nullptr;
        long _imageSize = 0;
        long _curlopt_low_speed_time_sec = 0;
        std::string _url;
        size_t  _last_write_time_secs;
        // std::atomic<bool> &_quit;
        std::map<std::string, std::string> _header;
        std::string _local_file_path;
        ssize_t _starting_byte_to_download = 0;
        ssize_t _content_length = -1;
        ssize_t _total_bytes_written = 0;
        bool _resume_broken_download = true;
        bool _verify_peer = true;
        bool _verify_host = true;
        std::string _basic_auth_username;
        std::string _basic_auth_password;
        std::function<bool (long curl_result, long http_status_code, size_t bytes_written, size_t content_length, const std::string &errmsg)> _progressfn = nullptr;
    };

    static size_t writeHeader(void *data, size_t size, size_t nmemb, void *stream);
    static size_t writeDataToFile(void *data, size_t size, size_t nmemb, void *stream);
    void finish(CurlContext &ctx);

    const cJSON* _config = nullptr;

public:
    CurlHelper(const cJSON* json);
    ~CurlHelper() = default;

    void startSession(std::function<
        bool (long curl_result, long http_status_code, ssize_t bytes_written, ssize_t content_length, const std::string &errmsg)> 
        progressfn = nullptr);
};
