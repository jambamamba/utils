#pragma once
#ifdef __cplusplus
extern "C" {
#endif

enum DebugLogLevelU {
    DebugLogLevel_DEBUG,
    DebugLogLevel_INFO,
    DebugLogLevel_WARNING,
    DebugLogLevel_FATAL,
    DebugLogLevel_NumLevels
};

//typedef struct DebugLoggerT {
//
//} DebugLogger;

typedef struct DebugLoggerCategoryT {
    int _category_id;
    const char *_category_name;
} DebugLoggerCategory;

typedef struct DebugLoggerQueueDataT {
    enum DebugLogLevelU _level;
    char _text[1024];
} DebugLoggerQueueData;

#define LOG_INIT(path) \
    debuglogger_set_log_dir(path);

#define LOG_CATEGORY(mnemonic, category_name) \
    static DebugLoggerCategory _category_##mnemonic = { ._category_id = 0, ._category_name = category_name }; \
    static int _category_id_##mnemonic;

#define LOG(level, mnemonic, format, ...) \
    if(_category_id_##mnemonic == 0) { \
        _category_id_##mnemonic = debuglogger_category(_category_##mnemonic._category_name ); \
        debuglogger_start_worker_thread(); \
    } \
    debuglogger_log(DebugLogLevel_##level, _category_id_##mnemonic, __FILE__, __LINE__, format __VA_OPT__(,) __VA_ARGS__);


void debuglogger_log(int/*DebugLogLevelU*/ level, int category_id, const char* file, int line_num, const char *format, ...);
int debuglogger_category(const char *category_name);
void debuglogger_start_worker_thread();
void debuglogger_set_log_dir(const char *path);

#ifdef __cplusplus
} //extern "C"
#endif