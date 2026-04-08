internal char *log_get_reset_color(Log_Context context)
{
    char *reset_color = "";
    if (context.enable_color_log || term_is_color_allowed())
    {
        reset_color = TERM_RESET;
    }
    return reset_color;
}

internal char *log_get_level_color(Log_Level level)
{
    char *level_color = "";
    switch (level)
    {
        case Log_Level_None:
        {
        } break;
        case Log_Level_Info:
        {
            level_color = TERM_FG_BLUE;
        } break;
        case Log_Level_Debug:
        {
            level_color = TERM_FG_MAGENTA;
        } break;
        case Log_Level_Warn:
        {
            level_color = TERM_FG_YELLOW;
        } break;
        case Log_Level_Error:
        {
            level_color = TERM_FG_RED;
        } break;
    }
    return level_color;
}
internal char *log_get_level_string(Log_Level level)
{
    char *level_string = "";
    switch (level)
    {
        case Log_Level_None:
        {
            level_string = "";
        } break;
        case Log_Level_Info:
        {
            level_string = "[INFO]";
        } break;
        case Log_Level_Debug:
        {
            level_string = "[DEBUG]";
        } break;
        case Log_Level_Warn:
        {
            level_string = "[WARN]";
        } break;
        case Log_Level_Error:
        {
            level_string = "[ERROR]";
        } break;
    }
    return level_string;
}

internal char *log_get_file_info_color(Log_Context context)
{
    char *line_info_color = "";
    if (context.enable_color_log || term_is_color_allowed())
    {
        line_info_color = TERM_FG_CYAN;
    }
    return line_info_color;
}

internal void log_print_color_level(Log_Context context, Log_Level level)
{
    char *level_string = log_get_level_string(level);
    char *level_color = "";
    if (context.enable_color_log && term_is_color_allowed())
    {
        level_color = log_get_level_color(level);
    }
    fmt_fprintf(context.file, "%s%s%s ", level_color, level_string, log_get_reset_color(context));
}

internal void log_vprintf(Log_Context context, Log_Level level, const char *format, va_list args)
{
    if (level >= context.level)
    {
        if (context.print_level_prefix)
        {
            log_print_color_level(context, level);
        }
        va_list args_copy;
        va_copy(args_copy, args);
            fmt_vfprintfln(context.file, format, args_copy);
        va_end(args_copy);
    }
}

internal Log_Context log_init(void)
{
    Log_Context context = ZERO_STRUCT;
    context.level = Log_Level_Info;
    context.file = OS_STDOUT;
    context.print_level_prefix = true;
    context.enable_color_log = true;
    return context;
}

internal void log_info(Log_Context context, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_vprintf(context, Log_Level_Info, format, args);
    va_end(args);
}
internal void log_debug(Log_Context context, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_vprintf(context, Log_Level_Debug, format, args);
    va_end(args);
}
internal void log_warn(Log_Context context, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_vprintf(context, Log_Level_Warn, format, args);
    va_end(args);
}
internal void log_error(Log_Context context, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_vprintf(context, Log_Level_Error, format, args);
    va_end(args);
}
