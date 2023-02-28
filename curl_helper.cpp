#include "curl_helper.h"

#include <chrono>

#include "file_utils.h"
#include "debug_logger.h"
#include "json_utils.h"

LOG_CATEGORY(MAIN, "MAIN")

CurlHelper::CurlContext::CurlContext(std::function
    <bool (long curl_result, long http_status_code, ssize_t bytes_written, ssize_t content_length)> progressfn)
    : _progressfn(progressfn)
{}

CurlHelper::CurlHelper(const cJSON* config) 
: _config(config)
{}

CurlHelper::CurlContext::~CurlContext()
{
  if(_memory){
    free(_memory);
  }
  _memory = nullptr;
  
  if (_filep){
    // fclose(_filep);
    _filep = nullptr;
  }
  curl_global_cleanup();
}

bool 
CurlHelper::CurlContext::loadFromJson(const cJSON* json){
    cJSON *server = cJSON_GetObjectItemCaseSensitive(json, "server");
    if(!server){
        LOG(FATAL, MAIN, "json config does not contain 'server' key");
        return false;
    }
    _url = cJSON_GetObjectItemCaseSensitive(server, "url")->valuestring;
    _local_file_path = cJSON_GetObjectItemCaseSensitive(server, "local_file_name")->valuestring;
    _resume_broken_download = cJSON_GetObjectItemCaseSensitive(server, "resume_broken_download") ?
        strcmp(cJSON_GetObjectItemCaseSensitive(server, "resume_broken_download")->valuestring, "true") == 0 : false;
    _basic_auth_username = cJSON_GetObjectItemCaseSensitive(server, "basic_auth_username") ? 
        cJSON_GetObjectItemCaseSensitive(server, "basic_auth_username")->valuestring : "";
    _basic_auth_password = cJSON_GetObjectItemCaseSensitive(server, "basic_auth_password") ?
        cJSON_GetObjectItemCaseSensitive(server, "basic_auth_password")->valuestring : "";

    if(!FileUtils::fileExists(_local_file_path)){
        _filep = fopen(_local_file_path.c_str(), "wb");
    }
    else if(_resume_broken_download) {
        _starting_byte_to_download = FileUtils::getFileSize(_local_file_path);
        _filep = fopen(_local_file_path.c_str(), "a+b");
        }
    else {
        _filep = fopen(_local_file_path.c_str(), "wb");
    }

    return true;
}

long
CurlHelper::CurlContext::httpStatusCode() const{
    long http_status_code = 0;
    curl_easy_getinfo (_handle, CURLINFO_RESPONSE_CODE, &http_status_code);
    return http_status_code;
}

size_t
CurlHelper::writeHeader(void *data, size_t size, size_t nmemb, void *stream){
  CurlContext *ctx = (CurlContext *)stream;
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_res, 
    ctx->httpStatusCode(), 
    ctx->_total_bytes_written, 
    ctx->_content_length)){
    return -1;
  }
  LOG(DEBUG, MAIN, "recvd header\n");
  char line[1024*10] = {0};
  memset(line, 0, sizeof(line));
  if(size * nmemb < sizeof(line)) {
    memcpy(line, data, size * nmemb);
  }
  else {
    LOG(FATAL, MAIN, "need more storage space for header line\n");
    return -1;
  }
  char *colon = strchr(line, ':');
  if(!colon){
    static int linenum = 0;
    char key[32] = {0};
    sprintf(key, "LINE%i", linenum++);
    ctx->_header[key] = line;
  }
  else{
    char key[1024] = {0};
    strncpy(key, line, colon - line);
    for(int i = 0; i < strlen(key); ++i) {
      if(key[i] == '\r' || key[i] == '\n'){
        key[i] = '\0';
      }
    }
    char value[1024*10] = {0};
    strncpy(value, colon + 2, nmemb - (colon - line + 2));
    for(int i = 0; i < strlen(key); ++i) {
      if(value[i] == '\r' || value[i] == '\n'){
        value[i] = '\0';
      }
    }
    ctx->_header[key] = value;
    if(strcmp(key, "Content-Length") == 0){
      if (1 != sscanf(value, "%zu", &ctx->_content_length)){
          LOG(FATAL, MAIN, "Failed to convert content length to integer\n");
      }
    }
  }

  return nmemb;
}

size_t 
CurlHelper::writeDataToFile(void *data, size_t size, size_t nmemb, void *stream){
  CurlContext *ctx = (CurlContext *)stream;
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_res, 
    ctx->httpStatusCode(), 
    ctx->_total_bytes_written, 
    ctx->_content_length)){
    return -1;
  }
  size_t bytes_written = fwrite(data, size, nmemb, (FILE *)ctx->_filep); 
  if (bytes_written == 0){
    LOG(FATAL, MAIN, "FAILED to write to file\n");
  }
  ctx->_last_write_time_secs =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
          .time_since_epoch()).count();      
  ctx->_total_bytes_written += bytes_written;
  LOG(DEBUG, MAIN, "wrote %zu/%zu bytes (%.02f%%) to file\n", 
    ctx->_total_bytes_written, 
    ctx->_content_length, 
    ctx->_content_length > 0 ? ctx->_total_bytes_written*100./ctx->_content_length : 0);
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_res, 
        ctx->httpStatusCode(), 
        ctx->_total_bytes_written, 
        ctx->_content_length)){
    return -1;
  }
  return bytes_written;
}

void 
CurlHelper::startSession(std::function<
    bool (long curl_result, long http_status_code, ssize_t bytes_written, ssize_t content_length)> progressfn){
  curl_global_init(CURL_GLOBAL_ALL);

  CurlHelper::CurlContext ctx(progressfn);
  if(!ctx.loadFromJson(_config)){
    return;
  }
  ctx._handle = curl_easy_init();

  // abort if slower than 30 bytes/sec during ctx._curlopt_low_speed_time seconds
  curl_easy_setopt(ctx._handle, CURLOPT_LOW_SPEED_TIME, ctx._curlopt_low_speed_time_sec);
  curl_easy_setopt(ctx._handle, CURLOPT_LOW_SPEED_LIMIT, 30L);
  LOG(DEBUG, MAIN, "set to abort if slower than 30 bytes/sec during %d seconds\n", ctx._curlopt_low_speed_time_sec);

  curl_easy_setopt(ctx._handle, CURLOPT_URL, ctx._url.c_str());

  curl_easy_setopt(ctx._handle, CURLOPT_RESUME_FROM, ctx._starting_byte_to_download);
  
  if(ctx._filep) {
    curl_easy_setopt(ctx._handle, CURLOPT_HEADERDATA, (void *)&ctx);
    curl_easy_setopt(ctx._handle, CURLOPT_HEADERFUNCTION, CurlHelper::writeHeader);
    curl_easy_setopt(ctx._handle, CURLOPT_WRITEDATA, (void *)&ctx);
    curl_easy_setopt(ctx._handle, CURLOPT_WRITEFUNCTION, CurlHelper::writeDataToFile);
    curl_easy_setopt(ctx._handle, CURLOPT_SSL_VERIFYPEER, false);   
    curl_easy_setopt(ctx._handle, CURLOPT_SSL_VERIFYHOST, false);
    // curl_easy_setopt(ctx._handle, CURLOPT_SSL_VERIFYSTATUS, false);

    if(ctx._basic_auth_username.size()) {
      std::string userpass = ctx._basic_auth_username;
      if(ctx._basic_auth_password.size()){
        userpass += ":" + ctx._basic_auth_password;
      }
      curl_easy_setopt(ctx._handle, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
      curl_easy_setopt(ctx._handle, CURLOPT_USERPWD, userpass.c_str());
    // curl_setopt(ctx._handle, CURLOPT_RETURNTRANSFER, true);
    }

    curl_easy_setopt(ctx._handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(ctx._handle, CURLOPT_FAILONERROR, 1);
    
    ctx._res = curl_easy_perform(ctx._handle);
    LOG(DEBUG, MAIN, "Download finished\n");
  }

  if (ctx._res != CURLE_OK){
    LOG(WARNING, MAIN, "libcurl error:%i '%s'\n", ctx.httpStatusCode(), curl_easy_strerror(ctx._res)); // FAILed
  }

  if(ctx._progressfn){
    ctx._progressfn(ctx._res, 
        ctx.httpStatusCode(), 
        ctx._total_bytes_written, 
        ctx._content_length);
  }

  return;
}