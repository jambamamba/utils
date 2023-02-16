#include <debug_logger.h>
#include <file_utils.h>

#include <dirent.h> 
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#if defined(WIN32) || defined(MSYS)
#include <windows.h>
#else
#include <sys/syscall.h>
#endif
#include <time.h>
#include <unistd.h>

//DebugLogger _debug_logger;
#pragma clang diagnostic ignored "-Wreturn-type"
#pragma clang diagnostic ignored "-Wimplicit-function-declaration"

static DebugLoggerCategory *_debug_logger_categories;
static int _num_categories;
static pthread_t _worker_thread_id;
static bool _die;
static DebugLoggerQueueData _queue[1024];
static int _queue_insert_idx;
static int _queue_remove_idx;
static pthread_mutex_t _queue_lock;
static pthread_cond_t _queue_signal = PTHREAD_COND_INITIALIZER;

extern char *__progname;

__attribute__((destructor)) void debuglogger_finish() 
{
    _die = true;
    pthread_join(_worker_thread_id, NULL);
}

static int getThreadId()
{
#if defined(WIN32) || defined(MSYS)
    return GetCurrentThreadId();
#else
    return syscall(SYS_gettid);
#endif
}
static char *debuglogger_file_name(char *logfile, size_t logfile_sz)
{
    struct stat st = {0};
    const char LOG_DIR[] = "c:/users/oosman"; //osm: should be configurable
    if (stat(LOG_DIR, &st) == -1) {
        makeDirectoryNonRecursive(LOG_DIR);
    }
    size_t nbytes = strlen(LOG_DIR) + strlen("/") + strlen(__progname) + strlen(".log") + 1;//snprintf(logfile, 0, "%s/%s.log", LOG_DIR, __progname) + 1;
    if (nbytes < logfile_sz) {
        memset(logfile, 0, logfile_sz);
        snprintf(logfile, nbytes, "%s/%s.log", LOG_DIR, __progname);
    }
    else {
        printf("FATAL: log file name is too long\n");
        abort();
    }
    return logfile;
}

static FILE* debuglogger_rename_file(FILE* fp, const char *orignal_file, const char *final_file)
{
    char logfile[1024] = {0};
    if(strcmp(debuglogger_file_name(logfile, sizeof(logfile)), orignal_file) == 0) {
        fclose(fp);
    }
    rename(orignal_file, final_file);
    if(strcmp(debuglogger_file_name(logfile, sizeof(logfile)), orignal_file) == 0) {
        fp = fopen(debuglogger_file_name(logfile, sizeof(logfile)), "a+t");
    }
    return fp;
}

static FILE *debuglogger_rotate_file(FILE* fp, const char *orignal_file, int idx)
{
    char logfile[1024] = {0};
    if(strcmp(debuglogger_file_name(logfile, sizeof(logfile)), orignal_file) == 0) {
        struct stat statbuf;
        if (stat(orignal_file, &statbuf) == -1) {
            FILE* fp1 = fopen("/tmp/crash", "wt");
            fprintf(fp1, "FATAL: Failed to get file size for '%s', errno %i %s\n", orignal_file, errno, strerror(errno));
            fclose(fp1);
            return fp;
        }
        #define MAX_FILE_SIZE 100 * 1024
        #define MAX_NUM_FILES 10
        if(statbuf.st_size < MAX_FILE_SIZE) {
            return fp;
        }
    }
    char final_file[1024] = {0};
    sprintf(final_file, "/tmp/%s.%02i.log", __progname, idx);

    if( access( final_file, F_OK ) != 0 ) {
        fp = debuglogger_rename_file(fp, orignal_file, final_file);
    }
    else if( idx == (MAX_NUM_FILES - 1)) {
        unlink(final_file);
        fp = debuglogger_rename_file(fp, orignal_file, final_file);
    }
    else {
        fp = debuglogger_rotate_file(fp, final_file, idx + 1);
        fp = debuglogger_rename_file(fp, orignal_file, final_file);
    }
    return fp;
}

static bool queueIsEmpty(bool lock){
    if(lock){
        pthread_mutex_lock(&_queue_lock);
    }
    bool ret = _queue_remove_idx == _queue_insert_idx;
    if(lock){
        pthread_mutex_unlock(&_queue_lock);
    }
    return ret;
}
static void *debuglogger_worker_thread(void * arg)
{
    pthread_setname_np(pthread_self(), "steno-logger");
    char logfile[1024] = {0};
    const char * logfilename = debuglogger_file_name(logfile, sizeof(logfile));
    FILE* fp = fopen(logfilename, "wt");
    if(!fp){
        fprintf(stdout, "FATAL Could not open log file '%s', errno [%i] %s\n", logfilename, errno, strerror(errno));
        abort();
    }
    static int queue_size = sizeof(_queue)/sizeof(_queue[0]);
    while(!_die) {
        pthread_mutex_lock(&_queue_lock);
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;
        if(queueIsEmpty(false) &&
            ETIMEDOUT == pthread_cond_timedwait(&_queue_signal, &_queue_lock, &ts)) {
            pthread_mutex_unlock(&_queue_lock);
            continue;
        }
        if(_queue_remove_idx > _queue_insert_idx) {
            for(int i = _queue_remove_idx; i < queue_size; ++i) {
                fprintf(fp, "%s", _queue[_queue_remove_idx]._text);
                fprintf(stdout, "%s", _queue[_queue_remove_idx]._text);
                _queue_remove_idx = (_queue_remove_idx + 1) % queue_size;
            }
        }
        for(int i = _queue_remove_idx; i < _queue_insert_idx; ++i) {
            fprintf(fp, "%s", _queue[_queue_remove_idx]._text);
            fprintf(stdout, "%s", _queue[_queue_remove_idx]._text);
            _queue_remove_idx = (_queue_remove_idx + 1) % queue_size;
        }      
        pthread_mutex_unlock(&_queue_lock);
        fflush(fp);
        fp = debuglogger_rotate_file(fp, logfilename, 1);
        if(!fp){
            fprintf(stdout, "Could not open log file %s, errno [%i] %s\n", logfilename, errno, strerror(errno));
            abort();
        }        
    }
}

static pthread_mutex_t _access_lock;

int debuglogger_category(const char *category_name)
{
#if defined(WIN32) || defined(MSYS)
    static bool initialized = false;
    if(!initialized) {
        initialized = true;
        pthread_mutex_init(&_access_lock, 0);
    }
#else
    if(!_access_lock.__data.__lock){
        pthread_mutex_init(&_access_lock, 0);
    }
#endif

    int category_id = 0;
    pthread_mutex_lock(&_access_lock);
    // std::scoped_lock lock(_access_lock);
    for(int i = 0; i < _num_categories; ++i) {
        if (strcmp(_debug_logger_categories[i]._category_name, category_name) == 0) {
            pthread_mutex_unlock(&_access_lock);
            return _debug_logger_categories[i]._category_id;
        }
    }
    _debug_logger_categories = (DebugLoggerCategory *)realloc(
        _debug_logger_categories, 
        sizeof(DebugLoggerCategory) * (_num_categories + 1));
    _debug_logger_categories[_num_categories]._category_id = _num_categories + 1;
    _debug_logger_categories[_num_categories]._category_name = category_name;
    _num_categories++;

    pthread_mutex_unlock(&_access_lock);
    return _debug_logger_categories[_num_categories-1]._category_id;
}

void debuglogger_start_worker_thread()
{
    if(_worker_thread_id == 0) {
        pthread_create(&_worker_thread_id, NULL, &debuglogger_worker_thread, NULL);
    }
}
static const char* debuglogger_category_name(int category_id) 
{
    if(category_id <= _num_categories) {
        return _debug_logger_categories[category_id - 1]._category_name;
    }
    else {
        return "DEFAULT";
    }
}

static long currentTimeMillis() 
{
  struct timeval time;
  gettimeofday(&time, NULL);

  return time.tv_sec * 1000 + time.tv_usec / 1000;
}
void debuglogger_log(int/*DebugLogLevelU*/ level, int category_id, const char* file, int line_num, const char *format, ...)
{
    time_t rawtime;
    struct tm *info;
    char time_stamp[80] = {0};
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(time_stamp, sizeof(time_stamp),"%x-%I:%M:%S%p", info);
    int sz = snprintf(NULL, 0, "%s.%li", time_stamp, currentTimeMillis());
    if(sz >= sizeof(time_stamp)){
        fprintf(stdout, "time_stamp line is too long at %i bytes, max line length limit is %i\n", sz, (int)sizeof(time_stamp)-1);
        abort();
    }
    snprintf(time_stamp, sizeof(time_stamp), "%s.%li", time_stamp, currentTimeMillis());
    
    if(level < DebugLogLevel_DEBUG || level > DebugLogLevel_FATAL) {
        fprintf(stdout, "invalid log level was used\n");
        abort();
    }
    static const char* LogLevels[DebugLogLevel_NumLevels] = {"DEBUG", "INFO", "WARNING", "FATAL"};
    const char *level_name = LogLevels[level];

  char buffer[sizeof(_queue[0])];
  va_list args;
  va_start (args, format);
  sz = snprintf(NULL, 0, "[%s] [%s:%s] [%s:%i:%li] ",
    time_stamp,
    level_name,
    debuglogger_category_name(category_id),
    file,
    line_num,
    getThreadId()
    );
  if (sz >= sizeof(_queue[0])){
    fprintf(stdout, "log line is too long at %i bytes, max line length limit is %i\n", sz, (int)sizeof(_queue[0])-1);
    abort();
  }
  snprintf(buffer, sizeof(_queue[0]), "[%s] [%s:%s] [%s:%i:%li] ",
    time_stamp,
    level_name,
    debuglogger_category_name(category_id),
    file,
    line_num,
    getThreadId()
    );
  vsnprintf (buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), format, args);
  va_end (args);

  static int queue_size = sizeof(_queue)/sizeof(_queue[0]);
  pthread_mutex_lock(&_queue_lock);
  memcpy(_queue[_queue_insert_idx]._text, buffer, sizeof(_queue[0]._text));
  _queue[_queue_insert_idx]._level = level;
  _queue_insert_idx = (_queue_insert_idx + 1) % queue_size;
  if(_queue_insert_idx == _queue_remove_idx) {
      _queue_remove_idx = (_queue_remove_idx + 1) % queue_size;
  }
  pthread_cond_signal(&_queue_signal);
  pthread_mutex_unlock(&_queue_lock);

  if(level == DebugLogLevel_FATAL) {
    while(!queueIsEmpty(true)){
        usleep(1000);
    }
    abort();
  }
}