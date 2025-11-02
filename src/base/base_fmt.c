// FPrint
// ============================================================================

internal U64 fmt_fprint(Os_File file, const char *string)
{
    U64 length = cstr8_length((U8 *)string);
    os_file_write(file, rng_1u64(0, length), (char *)string);
    return length;
}
internal U64 fmt_fprintln(Os_File file, const char *string)
{
    U64 length = cstr8_length((U8 *)string);
    fmt_fprint(file, string);
    fmt_fprint(file, "\n");
    return length;
}
internal U64 fmt_vfprintf(Os_File file, const char *format, va_list args)
{
    U64 written = 0;
    va_list args_copy;
    va_copy(args_copy, args);
        I32 needed = fmt_vsnprintf(NULL, 0, format, args_copy) + 1;
    va_end(args_copy);
    if (needed > 0) {
        char *buffer = alloc_make(_os_core_state.alloc, char, needed);
        va_copy(args_copy, args);
            written = fmt_vsprintf(buffer, format, args_copy);
        va_end(args_copy);
        os_file_write(file, rng_1u64(0, written), buffer);
        alloc_free(_os_core_state.alloc, buffer, needed);
    }
    return written;
}
internal U64 fmt_fprintf(Os_File file, const char *format, ...)
{
    U64 written = 0;
    va_list args;
    va_start(args, format);
        written = fmt_vfprintf(file, format, args);
    va_end(args);
    return written;
}
internal U64 fmt_vfprintfln(Os_File file, const char *format, va_list args)
{
    U64 written = 0;
    va_list args_copy;
    va_copy(args_copy, args);
        I32 needed = fmt_vsnprintf(NULL, 0, format, args_copy) + 2;
    va_end(args_copy);
    if (needed > 0) {
        char *buffer = alloc_make(_os_core_state.alloc, char, needed);
        va_copy(args_copy, args);
            written = fmt_vsprintf(buffer, format, args_copy);
        va_end(args_copy);
        buffer[written] = '\n';
        buffer[written+1] = '\0';
        os_file_write(file, rng_1u64(0, written+1), buffer);
        alloc_free(_os_core_state.alloc, buffer, needed);
    }
    return written;
}
internal U64 fmt_fprintfln(Os_File file, const char *format, ...)
{
    U64 written = 0;
    va_list args;
    va_start(args, format);
        written = fmt_vfprintfln(file, format, args);
    va_end(args);
    return written;
}

// Print
// ============================================================================

internal void fmt_print(const char *string)
{
    fmt_fprint(OS_STDOUT, string);
}
internal void fmt_println(const char *string)
{
    fmt_fprintln(OS_STDOUT, string);
}
internal void fmt_vprintf(const char *format, va_list args)
{
    fmt_vfprintf(OS_STDOUT, format, args);
}
internal void fmt_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        fmt_vprintf(format, args);
    va_end(args);
}
internal void fmt_vprintfln(const char *format, va_list args)
{
    fmt_vfprintfln(OS_STDOUT, format, args);
}
internal void fmt_printfln(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        fmt_vprintfln(format, args);
    va_end(args);
}

// EPrint
// ============================================================================

internal void fmt_eprint(const char *string)
{
    fmt_fprint(OS_STDERR, string);
}
internal void fmt_eprintln(const char *string)
{
    fmt_fprintln(OS_STDOUT, string);
}
internal void fmt_veprintf(const char *format, va_list args)
{
    fmt_vfprintf(OS_STDERR, format, args);
}
internal void fmt_eprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        fmt_veprintf(format, args);
    va_end(args);
}
internal void fmt_veprintfln(const char *format, va_list args)
{
    fmt_vfprintfln(OS_STDERR, format, args);
}
internal void fmt_eprintfln(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        fmt_veprintfln(format, args);
    va_end(args);
}
