#include "curl_helper.h"

#include <chrono>

#include "file_utils.h"
#include "debug_logger.h"
#include "json_utils.h"

LOG_CATEGORY(MAIN, "MAIN")

namespace {


std::string cURLcodeToString(CURLcode code){
  switch(code){
  case CURLE_OK: return "";
  case CURLE_UNSUPPORTED_PROTOCOL: return "1: CURLE_UNSUPPORTED_PROTOCOL";
  case CURLE_FAILED_INIT: return "2: CURLE_FAILED_INIT";
  case CURLE_URL_MALFORMAT: return "           /* 3 */";
  case CURLE_NOT_BUILT_IN: return "            /* 4 - [was obsoleted in August 2007 for 7.17.0, reused in April 2011 for 7.21.5] */";
  case CURLE_COULDNT_RESOLVE_PROXY: return "   /* 5 */";
  case CURLE_COULDNT_RESOLVE_HOST: return "    /* 6 CURLE_COULDNT_RESOLVE_HOST */";
  case CURLE_COULDNT_CONNECT: return "         /* 7 */";
  case CURLE_WEIRD_SERVER_REPLY: return "      /* 8 */";
  case CURLE_REMOTE_ACCESS_DENIED: return "    /* 9 a service was denied by the server due to lack of access - when login fails this is not returned. */";
  case CURLE_FTP_ACCEPT_FAILED: return "       /* 10 - [was obsoleted in April 2006 for 7.15.4, reused in Dec 2011 for 7.24.0]*/";
  case CURLE_FTP_WEIRD_PASS_REPLY: return "    /* 11 */";
  case CURLE_FTP_ACCEPT_TIMEOUT: return "      /* 12 - timeout occurred accepting server [was obsoleted in August 2007 for 7.17.0, reused in Dec 2011 for 7.24.0]*/";
  case CURLE_FTP_WEIRD_PASV_REPLY: return "    /* 13 */";
  case CURLE_FTP_WEIRD_227_FORMAT: return "    /* 14 */";
  case CURLE_FTP_CANT_GET_HOST: return "       /* 15 */";
  case CURLE_HTTP2: return "                   /* 16 - A problem in the http2 framing layer. [was obsoleted in August 2007 for 7.17.0, reused in July 2014 for 7.38.0] */";
  case CURLE_FTP_COULDNT_SET_TYPE: return "    /* 17 */";
  case CURLE_PARTIAL_FILE: return "            /* 18 */";
  case CURLE_FTP_COULDNT_RETR_FILE: return "   /* 19 */";
  case CURLE_OBSOLETE20: return "              /* 20 - NOT USED */";
  case CURLE_QUOTE_ERROR: return "             /* 21 - quote command failure */";
  case CURLE_HTTP_RETURNED_ERROR: return "     /* 22 */ CURLE_HTTP_RETURNED_ERROR";
  case CURLE_WRITE_ERROR: return "             /* 23 */";
  case CURLE_OBSOLETE24: return "              /* 24 - NOT USED */";
  case CURLE_UPLOAD_FAILED: return "           /* 25 - failed upload 'command' */";
  case CURLE_READ_ERROR: return "              /* 26 - couldn't open/read from file */";
  case CURLE_OUT_OF_MEMORY: return "           /* 27 */";
  case CURLE_OPERATION_TIMEDOUT: return "      /* 28 - the timeout time was reached */";
  case CURLE_OBSOLETE29: return "              /* 29 - NOT USED */";
  case CURLE_FTP_PORT_FAILED: return "         /* 30 - FTP PORT operation failed */";
  case CURLE_FTP_COULDNT_USE_REST: return "    /* 31 - the REST command failed */";
  case CURLE_OBSOLETE32: return "              /* 32 - NOT USED */";
  case CURLE_RANGE_ERROR: return "             /* 33 - RANGE 'command' didn't work */";
  case CURLE_HTTP_POST_ERROR: return "         /* 34 */";
  case CURLE_SSL_CONNECT_ERROR: return "       /* 35 - wrong when connecting with SSL */";
  case CURLE_BAD_DOWNLOAD_RESUME: return "     /* 36 - couldn't resume download */";
  case CURLE_FILE_COULDNT_READ_FILE: return "  /* 37 */";
  case CURLE_LDAP_CANNOT_BIND: return "        /* 38 */";
  case CURLE_LDAP_SEARCH_FAILED: return "      /* 39 */";
  case CURLE_OBSOLETE40: return "              /* 40 - NOT USED */";
  case CURLE_FUNCTION_NOT_FOUND: return "      /* 41 - NOT USED starting with 7.53.0 */";
  case CURLE_ABORTED_BY_CALLBACK: return "     /* 42 */";
  case CURLE_BAD_FUNCTION_ARGUMENT: return "   /* 43 */";
  case CURLE_OBSOLETE44: return "              /* 44 - NOT USED */";
  case CURLE_INTERFACE_FAILED: return "        /* 45 - CURLOPT_INTERFACE failed */";
  case CURLE_OBSOLETE46: return "              /* 46 - NOT USED */";
  case CURLE_TOO_MANY_REDIRECTS: return "      /* 47 - catch endless re-direct loops */";
  case CURLE_UNKNOWN_OPTION: return "          /* 48 - User specified an unknown option */";
  case CURLE_SETOPT_OPTION_SYNTAX: return "    /* 49 - Malformed setopt option */";
  case CURLE_OBSOLETE50: return "              /* 50 - NOT USED */";
  case CURLE_OBSOLETE51: return "              /* 51 - NOT USED */";
  case CURLE_GOT_NOTHING: return "             /* 52 - when this is a specific error */";
  case CURLE_SSL_ENGINE_NOTFOUND: return "     /* 53 - SSL crypto engine not found */";
  case CURLE_SSL_ENGINE_SETFAILED: return "    /* 54 - can not set SSL crypto engine as default */";
  case CURLE_SEND_ERROR: return "              /* 55 - failed sending network data */";
  case CURLE_RECV_ERROR: return "              /* 56 - failure in receiving network data */";
  case CURLE_OBSOLETE57: return "              /* 57 - NOT IN USE */";
  case CURLE_SSL_CERTPROBLEM: return "         /* 58 - problem with the local certificate */";
  case CURLE_SSL_CIPHER: return "              /* 59 - couldn't use specified cipher */";
  case CURLE_PEER_FAILED_VERIFICATION: return "/* 60 - peer's certificate or fingerprint wasn't verified fine */";
  case CURLE_BAD_CONTENT_ENCODING: return "    /* 61 - Unrecognized/bad encoding */";
  case CURLE_OBSOLETE62: return "              /* 62 - NOT IN USE since 7.82.0 */";
  case CURLE_FILESIZE_EXCEEDED: return "       /* 63 - Maximum file size exceeded */";
  case CURLE_USE_SSL_FAILED: return "          /* 64 - Requested FTP SSL level failed */";
  case CURLE_SEND_FAIL_REWIND: return "        /* 65 - Sending the data requires a rewind that failed */";
  case CURLE_SSL_ENGINE_INITFAILED: return "   /* 66 - failed to initialise ENGINE */";
  case CURLE_LOGIN_DENIED: return "            /* 67 - user: return password or similar was not accepted and we failed to login */";
  case CURLE_TFTP_NOTFOUND: return "           /* 68 - file not found on server */";
  case CURLE_TFTP_PERM: return "               /* 69 - permission problem on server */";
  case CURLE_REMOTE_DISK_FULL: return "        /* 70 - out of disk space on server */";
  case CURLE_TFTP_ILLEGAL: return "            /* 71 - Illegal TFTP operation */";
  case CURLE_TFTP_UNKNOWNID: return "          /* 72 - Unknown transfer ID */";
  case CURLE_REMOTE_FILE_EXISTS: return "      /* 73 - File already exists */";
  case CURLE_TFTP_NOSUCHUSER: return "         /* 74 - No such user */";
  case CURLE_OBSOLETE75: return "              /* 75 - NOT IN USE since 7.82.0 */";
  case CURLE_OBSOLETE76: return "              /* 76 - NOT IN USE since 7.82.0 */";
  case CURLE_SSL_CACERT_BADFILE: return "      /* 77 - could not load CACERT file: return ' missing' or wrong format */";
  case CURLE_REMOTE_FILE_NOT_FOUND: return "   /* 78 - remote file not found */";
  case CURLE_SSH: return "                     /* 79 - error from the SSH layer: return ' somewhat' generic so the error message will be of interest when this has happened */";
  case CURLE_SSL_SHUTDOWN_FAILED: return "     /* 80 - Failed to shut down the SSL connection */";
  case CURLE_AGAIN: return "                   /* 81 - socket is not ready for send/recv: return '' wait till it's ready and try again (Added in 7.18.2) */";
  case CURLE_SSL_CRL_BADFILE: return "         /* 82 - could not load CRL file: return '' missing or wrong format (Added in 7.19.0) */";
  case CURLE_SSL_ISSUER_ERROR: return "        /* 83 - Issuer check failed.  (Added in7.19.0) */";
  case CURLE_FTP_PRET_FAILED: return "         /* 84 - a PRET command failed */";
  case CURLE_RTSP_CSEQ_ERROR: return "         /* 85 - mismatch of RTSP CSeq numbers */";
  case CURLE_RTSP_SESSION_ERROR: return "      /* 86 - mismatch of RTSP Session Ids */";
  case CURLE_FTP_BAD_FILE_LIST: return "       /* 87 - unable to parse FTP file list */";
  case CURLE_CHUNK_FAILED: return "            /* 88 - chunk callback reported error */";
  case CURLE_NO_CONNECTION_AVAILABLE: return " /* 89 - No connection available: return '' thesession will be queued */";
  case CURLE_SSL_PINNEDPUBKEYNOTMATCH: return " /* 90 - specified pinned public key did not match */";
  case CURLE_SSL_INVALIDCERTSTATUS: return "   /* 91 - invalid certificate status */";
  case CURLE_HTTP2_STREAM: return "            /* 92 - stream error in HTTP/2 framing layer */";
  case CURLE_RECURSIVE_API_CALL: return "      /* 93 - an api function was called from inside a callback */";
  case CURLE_AUTH_ERROR: return "              /* 94 - an authentication function returned an error */";
  case CURLE_HTTP3: return "                   /* 95 - An HTTP/3 layer problem */";
  case CURLE_QUIC_CONNECT_ERROR: return "      /* 96 - QUIC connection error */";
  case CURLE_PROXY: return "                   /* 97 - proxy handshake error */";
  case CURLE_SSL_CLIENTCERT: return "          /* 98 - client-side certificate required */";
  case CURLE_UNRECOVERABLE_POLL: return "      /* 99 - poll/select returned fatal error */";
  case CURL_LAST: return  "/* never use! */";
  }
}
}//namespace


CurlHelper::CurlContext::CurlContext(CurlProgresCb progressfn)
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

std::string 
CurlHelper::CurlContext::errorToString() const {
  return cURLcodeToString(_code);
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

    _basic_auth_username = cJSON_GetObjectItemCaseSensitive(server, "basic_auth_username") ? 
        cJSON_GetObjectItemCaseSensitive(server, "basic_auth_username")->valuestring : "";
    _basic_auth_password = cJSON_GetObjectItemCaseSensitive(server, "basic_auth_password") ?
        cJSON_GetObjectItemCaseSensitive(server, "basic_auth_password")->valuestring : "";

    _resume_broken_download = cJSON_GetObjectItemCaseSensitive(server, "resume_broken_download") ?
        strcmp(cJSON_GetObjectItemCaseSensitive(server, "resume_broken_download")->valuestring, "true") == 0 : false;
    _verify_peer = cJSON_GetObjectItemCaseSensitive(server, "verify_peer") ?
        strcmp(cJSON_GetObjectItemCaseSensitive(server, "verify_peer")->valuestring, "true") == 0 : false;
    _verify_host = cJSON_GetObjectItemCaseSensitive(server, "verify_host") ?
        strcmp(cJSON_GetObjectItemCaseSensitive(server, "verify_host")->valuestring, "true") == 0 : false;
        
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
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_code, 
    ctx->httpStatusCode(),
    ctx->_total_bytes_written, 
    ctx->_content_length,
    ctx->errorToString())){
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
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_code, 
    ctx->httpStatusCode(),
    ctx->_total_bytes_written, 
    ctx->_content_length,
    ctx->errorToString())){
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
  if (ctx->_progressfn && !ctx->_progressfn(ctx->_code, 
        ctx->httpStatusCode(),
        ctx->_total_bytes_written, 
        ctx->_content_length,
        ctx->errorToString())){
    return -1;
  }
  return bytes_written;
}

static int 
curlProgress(void *clientp,
                      double dltotal,
                      double dlnow,
                      double ultotal,
                      double ulnow){
  struct progress *memory = (struct progress *)clientp;
  return 0;
}

static int 
curlTracer(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *clientp){
  const char *text;
  (void)handle; /* prevent compiler warning */
  (void)clientp;
 
  switch (type) {
  case CURLINFO_TEXT:
    LOG(DEBUG, MAIN, "== Info: %s\n", data);
    // fwrite(data, size, 1, stderr);
  default: /* in case a new one is introduced to shock us */
    return 0;
 
  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }
  LOG(DEBUG, MAIN, "== Info: %s\n", text);
  return 0;
}
void 
CurlHelper::startSession(CurlProgresCb progressfn){
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

    curl_easy_setopt(ctx._handle, CURLOPT_SSL_VERIFYPEER, ctx._verify_peer);   
    curl_easy_setopt(ctx._handle, CURLOPT_SSL_VERIFYHOST, ctx._verify_host);
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
    
    curl_easy_setopt(ctx._handle, CURLOPT_FOLLOWLOCATION, 1L); //follow redirection if we get HTTP 302

    // ((struct Curl_easy*)(ctx._handle))->set.fprogress = 0;
    // ((struct Curl_easy*)(ctx._handle))->set.fdebug = 0;
    curl_easy_setopt(ctx._handle, CURLOPT_CONNECTTIMEOUT_MS, 3000);
    curl_easy_setopt(ctx._handle, CURLOPT_PROGRESSFUNCTION, curlProgress);
    curl_easy_setopt(ctx._handle, CURLOPT_DEBUGFUNCTION, curlTracer);
    ctx._code = curl_easy_perform(ctx._handle);
    LOG(DEBUG, MAIN, "Download finished\n");
  }

  if (ctx._code != CURLE_OK){
    LOG(WARNING, MAIN, "HTTP:%i, libcurl error:[%i] '%s'\n", 
      ctx.httpStatusCode(), 
      ctx._code, 
      curl_easy_strerror(ctx._code)); // FAILed
  }

  if(ctx._progressfn){
    ctx._progressfn(
        ctx._code, 
        ctx.httpStatusCode(),
        ctx._total_bytes_written, 
        ctx._content_length,
        ctx.errorToString());
  }

  return;
}

