internal void * os_memory_alloc(uint64_t size)
{
    void *result = os_memory_create(size);
    os_memory_commit(result, size);
    return result;
}

internal Str8 os_file_read_str(Os_File file, Rng1_U64 range, Alloc alloc)
{
    Str8 result;
    result.length = dim1_u64(range);
    result.cstr = alloc_make(alloc, uint8_t, result.length);
    uint64_t actual_read_size = os_file_read(file, range, result.cstr);
    if(actual_read_size < result.length)
    {
        alloc_free(alloc, result.cstr, result.length);
        result.length = actual_read_size;
    }
    return result;
}

internal Str8 os_file_read_str_full(Os_File file, Alloc alloc)
{
    Str8 result;
    Os_FileProperties prop = os_file_properties(file);
    result = os_file_read_str(file, rng1_u64(0, prop.size), alloc);
    return result;
}

internal Str8Array *os_args_get(void)
{
    return &_os_core_state.args;
}

internal Str8 *os_program_path_get(void)
{
    return &_os_core_state.args.v[0];
}

internal void os_entry_point(void)
{
    _os_core_state.log_context = log_init();
#if BUILD_DEBUG
    _os_core_state.log_context.level = Log_Level_Info;
    fmt_println("# Program Output ============================================================ #");
#else
    _os_core_state.log_context.level = Log_Level_None;
#endif
    entry_point();
}
