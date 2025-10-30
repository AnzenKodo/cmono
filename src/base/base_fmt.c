internal U64 fmt_vsprintfln(char *buffer, const char *format, va_list args)
{
    U64 written = fmt_vsprintf(buffer, format, args);
    buffer[written-1] = '\n';
    return written;
}
internal U64 fmt_sprintfln(void *buffer, const char *format, ...)
{
    va_list args;
    va_start(args, format);
        U64 written = fmt_vsprintfln(buffer, format, args);
    va_end(args);
    return written;
}

internal U64 fmt_fprint(Os_File file, const char *string)
{
    U64 length = cstr8_length((U8 *)string);
    os_file_write(file, rng_1u64(0, length), (char *)string);
    return length;
}
internal U64 vfmt_fprintf(Os_File file, const char *format, va_list args)
{
    U64 written = 0;
    I32 needed = fmt_vsnprintf(NULL, 0, format, args) + 1;
    if (needed > 0) {
        char *buffer = alloc_make(os_core_state.alloc, char, needed);
        written = fmt_vsprintf(buffer, format, args);
        os_file_write(file, rng_1u64(0, written), buffer);
        alloc_free(os_core_state.alloc, buffer, needed);
    }
    return written;
}
internal U64 fmt_fprintf(Os_File file, const char *format, ...)
{
    U64 written = 0;
    va_list args;
    va_start(args, format);
        written = vfmt_fprintf(file, format, args);
    va_end(args);
    return written;
}
internal U64 vfmt_fprintfln(Os_File file, const char *format, va_list args)
{
    U64 written = 0;
    I32 needed = fmt_vsnprintf(NULL, 0, format, args) + 1;
    if (needed > 0) {
        char *buffer = alloc_make(os_core_state.alloc, char, needed);
        written = fmt_vsprintfln(buffer, format, args);
        os_file_write(file, rng_1u64(0, written), buffer);
        alloc_free(os_core_state.alloc, buffer, needed);
    }
    return written;
}
internal U64 fmt_fprintfln(Os_File file, const char *format, ...)
{
    U64 written = 0;
    va_list args;
    va_start(args, format);
        written = vfmt_fprintfln(file, format, args);
    va_end(args);
    return written;
}


internal void fmt_print(const char *string)
{
    fmt_fprint(OS_STDOUT, string);
}
internal void vfmt_printf(const char *format, va_list args)
{
    vfmt_fprintf(OS_STDOUT, format, args);
}
internal void fmt_printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        vfmt_printf(format, args);
    va_end(args);
}
internal void vfmt_printfln(const char *format, va_list args)
{
    vfmt_fprintfln(OS_STDOUT, format, args);
}
internal void fmt_printfln(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        vfmt_printfln(format, args);
    va_end(args);
}

internal void fmt_eprint(const char *string)
{
    fmt_fprint(OS_STDERR, string);
}
internal void vfmt_eprintf(const char *format, va_list args)
{
    vfmt_fprintf(OS_STDERR, format, args);
}
internal void fmt_eprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        vfmt_eprintf(format, args);
    va_end(args);
}
internal void vfmt_eprintfln(const char *format, va_list args)
{
    vfmt_fprintfln(OS_STDERR, format, args);
}
internal void fmt_eprintfln(const char *format, ...)
{
    va_list args;
    va_start(args, format);
        vfmt_eprintfln(format, args);
    va_end(args);
}
