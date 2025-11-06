internal char *_log_get_level_string(Log_Level level)
{
    char *level_string;
    switch (level)
    {
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
        default:
        {
        }
    }
    return level_string;
}
internal char *_log_get_level_color(Log_Level level)
{
    char *level_color = 0;
    switch (level)
    {
        case Log_Level_Info:
        {
            level_color = ANSI_FG_BLUE;
        } break;
        case Log_Level_Debug:
        {
            level_color = ANSI_FG_MAGENTA;
        } break;
        case Log_Level_Warn:
        {
            level_color = ANSI_FG_YELLOW;
        } break;
        case Log_Level_Error:
        {
            level_color = ANSI_FG_RED;
        } break;
        default:
        {
        }
    }
    return level_color;
}

internal void log_printf(Log_Config config, Log_Level level, const char *format, va_list args)
{
    if (level >= config.level)
    {
        char *level_string = _log_get_level_string(level);
        char *level_color = "";
        if (config.color_log)
        {
            char *level_color = _log_get_level_color(level);
        }
        if (config.print_level_prefix)
        {
            fmt_fprintf(config.file, "%s%s"ANSI_RESET" ", level_color, level_string);
        }
        va_list args_copy;
        va_copy(args_copy, args);
            fmt_vfprintfln(config.file, format, args_copy);
        va_end(args_copy);
    }
}

internal Log_Config log_init(void)
{
    Log_Config config = ZERO_STRUCT;
    config.level = Log_Level_Info;
    config.file = OS_STDOUT;
    config.print_level_prefix = true;
    config.color_log = true;
    return config;
}

internal void log_info(Log_Config config, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_printf(config, Log_Level_Info, format, args);
    va_end(args);
}
internal void log_debug(Log_Config config, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_printf(config, Log_Level_Debug, format, args);
    va_end(args);
}
internal void log_warn(Log_Config config, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_printf(config, Log_Level_Warn, format, args);
    va_end(args);
}
internal void log_error(Log_Config config, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        log_printf(config, Log_Level_Error, format, args);
    va_end(args);
}
