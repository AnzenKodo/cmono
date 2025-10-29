internal void fmt_fprintfv(Os_File file, const char *format, va_list args)
{
}

internal void fmt_fprintf(Os_File file, const char *format, ...)
{
}

internal void fmt_printfv(const char *format, va_list args)
{
}

internal void fmt_printf(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    I32 needed = fmt_vsnprintf(NULL, 0, format, args);
    needed += 1;
    va_end(args);

    if (needed > 0) {
        char *buffer = alloc_make(os_core_state.alloc, char, needed);

        va_start(args, format);
        int len = fmt_vsprintf(buffer, format, args);  // Safe: buffer is large enough
        va_end(args);

        write(STDOUT_FILENO, buffer, len);
        alloc_free(os_core_state.alloc, buffer, needed);
    }
}

internal void fmt_eprintf(const char *format, va_list args)
{
}
