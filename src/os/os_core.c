internal void * os_memory_alloc(U64 size)
{
    void *result = os_memory_create(size);
    os_memory_commit(result, size);
    return result;
}

internal Str8 os_file_read_str(Os_File file, Rng1U64 range, Alloc alloc)
{
    Str8 result;
    result.size = dim_1u64(range);
    result.cstr = alloc_make(alloc, U8, result.size);
    U64 actual_read_size = os_file_read(file, range, result.cstr);
    if(actual_read_size < result.size)
    {
        alloc_free(alloc, result.cstr, result.size);
        result.size = actual_read_size;
    }
    return result;
}

internal Str8 os_file_read_str_full(Os_File file, Alloc alloc)
{
    Str8 result;
    Os_FileProperties prop = os_file_properties(file);
    result = os_file_read_str(file, rng_1u64(0, prop.size), alloc);
    return result;
}

internal Str8Array *os_args_get(void)
{
    return &os_core_state.args;
}

internal void os_entry_point(int argc, char *argv[])
{
    U64 size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);

    os_core_state.args = str8_array_reserve(alloc, argc);
    os_core_state.alloc = alloc;
    for(int i = 0; i < argc; i += 1)
    {
        Str8 str = str8_from_cstr(argv[i]);
        str8_array_append(&os_core_state.args, str);
    }

    entry_point();
}
