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

typedef struct Log_Context Log_Context;
struct Log_Context {
    Log_Level level;
    Os_File file;
    bool print_level_prefix;
    bool enable_color_log;
};

// Functions
//=============================================================================

internal char *_log_get_level_string(Log_Level level);
internal char *_log_get_level_color(Log_Level level);
internal void log_printf(Log_Context context, Log_Level level, const char *format, va_list args);
internal Log_Context log_init(void);
internal void log_info(Log_Context context, const char *format, ...);
internal void log_debug(Log_Context context, const char *format, ...);
internal void log_warn(Log_Context context, const char *format, ...);
internal void log_error(Log_Context context, const char *format, ...);

#define LogPrintfLine(context, level, format, ...) do { \
    char *level_string = _log_get_level_string(level); \
    char *level_color = ""; \
    char *line_info_color = ""; \
    if (context.enable_color_log || term_is_color_allowed()) \
    { \
        level_color = _log_get_level_color(level); \
        line_info_color = TERM_FG_CYAN; \
    } \
    fmt_fprintf(context.file, "%s%s"TERM_RESET"%s[%s:%d] "TERM_RESET, level_color, level_string, line_info_color, FILE_NAME, LINE_NUMBER); \
    fmt_fprintfln(context.file, format, ##__VA_ARGS__); \
} while(0)
#define LogInfoLine(context, format, ...) do { \
    LogPrintfLine(context, Log_Level_Info, format, ##__VA_ARGS__); \
} while(0)
#define LogDebugLine(context, format, ...) do { \
    LogPrintfLine(context, Log_Level_Debug, format, ##__VA_ARGS__); \
} while(0)
#define LogWarnLine(context, format, ...) do { \
    LogPrintfLine(context, Log_Level_Warn, format, ##__VA_ARGS__); \
} while(0)
#define LogErrorLine(context, format, ...) do { \
    LogPrintfLine(context, Log_Level_Error, format, ##__VA_ARGS__); \
} while(0)



#endif // BASE_LOG_H
