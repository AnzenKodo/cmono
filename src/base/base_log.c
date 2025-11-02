internal void log_printf(Log_Config config, Log_Level level, const char *format, va_list args)
{
    if (level >= config.level) {
        char *level_string;
        switch (level)
        {
            case Log_Level_Info:
            {
                level_string = "[INFO] ";
            } break;
            case Log_Level_Debug:
            {
                level_string = "[DEBUG] ";
            } break;
            case Log_Level_Warn:
            {
                level_string = "[WARN] ";
            } break;
            case Log_Level_Error:
            {
                level_string = "[ERROR] ";
            } break;
            default:
            {
            }
        }
        if (config.print_level_prefix)
        {
            fmt_fprint(config.file, level_string);
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
