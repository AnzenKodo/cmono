// Helpers Functions
//=============================================================================


// Memory Allocation
//=============================================================================

internal void * os_memory_create(U64 size)
{
    void *result = 0;
    return result;
}

internal bool os_memory_commit(void *ptr, U64 size)
{
    return true;
}

internal void os_memory_decommit(void *ptr, U64 size)
{
}

internal void os_memory_free(void *ptr, U64 size)
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

internal U64 os_file_read(Os_File file, Rng1U64 rng, void *out_data)
{
    return 0;
}

internal U64 os_file_write(Os_File file, Rng1U64 rng, void *data)
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

internal void os_exit(I32 exit_code)
{
}

// Time
//=============================================================================

internal U32 os_now_unix(void)
{
    return 0;
}

internal U64 os_now_microsec(void)
{
    return 0;
}

internal void os_sleep_microsec(U64 micosec)
{
}

internal void os_sleep_millisec(U32 millisec)
{
}

// OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    os_entry_point(argc, argv);
}
