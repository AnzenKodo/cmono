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
    bool color_log;
};

// Functions
//=============================================================================

internal char *_log_get_level_string(Log_Level level);
internal char *_log_get_level_color(Log_Level level);
internal void log_printf(Log_Config config, Log_Level level, const char *format, va_list args);
internal Log_Config log_init(void);
internal void log_info(Log_Config config, const char *format, ...);
internal void log_debug(Log_Config config, const char *format, ...);
internal void log_warn(Log_Config config, const char *format, ...);
internal void log_error(Log_Config config, const char *format, ...);

#define LogPrintfLine(config, level, format, ...) do { \
    char *level_string = _log_get_level_string(level); \
    char *level_color = ""; \
    char *line_info_color = ""; \
    if (config.color_log) \
    { \
        level_color = _log_get_level_color(level); \
        line_info_color = ANSI_FG_CYAN; \
    } \
    fmt_fprintf(config.file, "%s%s"ANSI_RESET"%s[%s:%d] "ANSI_RESET, level_color, level_string, line_info_color, FILE_NAME, LINE_NUMBER); \
    fmt_fprintfln(config.file, format, ##__VA_ARGS__); \
} while(0)
#define LogInfoLine(config, format, ...) do { \
    LogPrintfLine(config, Log_Level_Info, format, ##__VA_ARGS__); \
} while(0)
#define LogDebugLine(config, format, ...) do { \
    LogPrintfLine(config, Log_Level_Debug, format, ##__VA_ARGS__); \
} while(0)
#define LogWarnLine(config, format, ...) do { \
    LogPrintfLine(config, Log_Level_Warn, format, ##__VA_ARGS__); \
} while(0)
#define LogErrorLine(config, format, ...) do { \
    LogPrintfLine(config, Log_Level_Error, format, ##__VA_ARGS__); \
} while(0)



#endif // BASE_LOG_H
