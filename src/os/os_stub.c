// Helpers Functions
//=============================================================================


// Memory Allocation
//=============================================================================

internal void * os_memory_create(uint64_t size)
{
    void *result = 0;
    return result;
}

internal bool os_memory_commit(void *ptr, uint64_t size)
{
    return true;
}

internal void os_memory_decommit(void *ptr, uint64_t size)
{
}

internal void os_memory_free(void *ptr, uint64_t size)
{
}

// File System
//=============================================================================

internal Os_File os_file_open(Str8 path, Os_AccessFlags flags)
{
    return 0;
}

internal void os_file_close(Os_File file)
{
}

internal uint64_t os_file_read(Os_File file, Rng1_U64 rng, void *out_data)
{
    return 0;
}

internal uint64_t os_file_write(Os_File file, Rng1_U64 rng, void *data)
{
    return 0;
}

internal Os_FileProperties os_file_properties(Os_File file)
{
    Os_FileProperties props = ZERO_STRUCT;
    return props;
}

internal bool os_dir_make(Str8 path)
{
    return false;
}

// Exit
//=============================================================================

internal void os_exit(int32_t exit_code)
{
}

// Time
//=============================================================================

internal uint32_t os_now_unix(void)
{
    return 0;
}

internal uint64_t os_now_microsec(void)
{
    return 0;
}

internal void os_sleep_microsec(uint64_t micosec)
{
}

internal void os_sleep_millisec(uint32_t millisec)
{
}

// OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    os_entry_point(argc, argv);
}
