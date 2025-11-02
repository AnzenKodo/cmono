#ifndef BASE_LOG_H
#define BASE_LOG_H

typedef enum Log_Level {
    Log_Level_None,
    Log_Level_Info,
    Log_Level_Debug,
    Log_Level_Warn,
    Log_Level_Error,
    Log_Level_COUNT,
} Log_Level;

typedef struct Log_Config Log_Config;
struct Log_Config {
    Log_Level level;
    Os_File file;
    bool print_level_prefix;
};

// Functions
//=============================================================================

internal void log_printf(Log_Config config, Log_Level level, const char *format, va_list args);
internal Log_Config log_init(void);
internal void log_info(Log_Config config, const char *format, ...);
internal void log_debug(Log_Config config, const char *format, ...);
internal void log_warn(Log_Config config, const char *format, ...);
internal void log_error(Log_Config config, const char *format, ...);

#endif // BASE_LOG_H
