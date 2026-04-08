#ifndef BASE_LOG_H
#define BASE_LOG_H

//~ ak: Types
//=============================================================================

typedef enum Log_Level {
    Log_Level_None,
    Log_Level_Info,
    Log_Level_Debug,
    Log_Level_Warn,
    Log_Level_Error,
} Log_Level;

typedef struct Log_Context Log_Context;
struct Log_Context {
    Log_Level level;
    Os_File file;
    bool print_level_prefix;
    bool enable_color_log;
};

//~ ak: Functions
//=============================================================================

//~ ak: Helper Functions ======================================================

internal char *log_get_reset_color(Log_Context *context);
internal char *log_get_level_color(Log_Level level);
internal char *log_get_level_string(Log_Level level);
internal char *log_get_file_info_color(Log_Context *context);
internal void log_print_color_level(Log_Context *context, Log_Level level);

//~ ak: Initialization Functions ==============================================

internal Log_Context log_init(void);

//~ ak: Log Level Print Functions =============================================

internal void log_vprintf(Log_Context *context, Log_Level level, const char *format, va_list args);
internal void log_info(Log_Context *context, const char *format, ...) FmtTypeCheck(2, 3);
internal void log_debug(Log_Context *context, const char *format, ...) FmtTypeCheck(2, 3);
internal void log_warn(Log_Context *context, const char *format, ...) FmtTypeCheck(2, 3);
internal void log_error(Log_Context *context, const char *format, ...) FmtTypeCheck(2, 3);

//~ ak: Macros
//=============================================================================

#define LogPrintfLine(context, level, format, ...) do { \
    char *line_info_color = log_get_file_info_color(context, level); \
    fmt_fprintf(context.file, "%s%s:%d%s ", line_info_color, FILE_NAME, LINE_NUMBER, log_get_reset_color(context)); \
    log_print_color_level(context, level) \
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
