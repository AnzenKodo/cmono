//- ak: File System Helpers ===================================================

/// Handle Helpers

internal bool fs_file_match(Fs_File a, Fs_File b)
{
    bool result = MemMatchStruct(&a, &b);
    return result;
}

internal bool fs_file_is_zero(Fs_File file)
{
    bool result = fs_file_match(file, StructZeroType(Fs_File));
    return result;
}

/// U8Array Helpers

internal U8Array fs_read_data_from_file_range(Arena *arena, Fs_File file, Rng1_U64 range)
{
    size_t pre_pos = arena_pos(arena);
    U8Array result;
    result.size = dim_rng1(range);
    result.length = result.size;
    result.v = arena_push_nz(arena, uint8_t, result.size);
    size_t actual_read_size = fs_file_read(file, range, result.v);
    if(actual_read_size < result.size)
    {
        arena_pop_to(arena, pre_pos + actual_read_size);
        result.length = actual_read_size;
    }
    return result;
}

internal U8Array fs_read_data_from_file_path(Str8 path, Arena *arena)
{
    Fs_File file = fs_file_open(path, Fs_Access_Flag_Read|Fs_Access_Flag_ShareRead);
    Fs_File_Properties props = fs_file_properties_from_file(file);
    U8Array data = fs_read_data_from_file_range(arena, file, rng1_u64(0, props.size));
    fs_file_close(file);
    return data;
}

internal bool fs_write_data_to_file_path(Str8 path, U8Array data)
{
    bool good = 0;
    Fs_File file = fs_file_open(path, Fs_Access_Flag_Write);
    if(!fs_file_is_zero(file))
    {
        size_t bytes_written = fs_file_write(file, data.v, rng1_u64(0, data.size));
        good = (bytes_written == data.size);
        fs_file_close(file);
    }
    return good;
}

internal bool fs_append_data_to_file_path(Str8 path, U8Array data)
{
    bool good = 0;
    if(data.length != 0)
    {
        Fs_File file = fs_file_open(path, Fs_Access_Flag_Write|Fs_Access_Flag_Append);
        if(!fs_file_is_zero(file))
        {
            size_t pos = fs_file_properties_from_file(file).size;
            size_t bytes_written = fs_file_write(file, data.v, rng1_u64(pos, pos+data.size));
            good = (bytes_written == data.size);
            fs_file_close(file);
        }
    }
    return good;
}

/// Str8 Helpers

internal Str8 fs_read_str_from_file_path(Str8 path, Arena *arena);
internal bool fs_write_str_to_file_path(Str8 path, U8Array data);
internal bool fs_write_str_list_to_file_path(Str8 path, Str8_List list);
