// Helpers Functions
//=============================================================================

internal U32 os_w32_unix_time_from_file_time(FILETIME file_time)
{
    U64 win32_time = ((U64)file_time.dwHighDateTime << 32) | file_time.dwLowDateTime;
    U64 unix_time64 = ((win32_time - 0x19DB1DED53E8000ULL) / 10000000);

    Assert(unix_time64 <= max_u32);
    U32 unix_time32 = (U32)unix_time64;

    return unix_time32;
}

internal FilePropertyFlags os_w32_file_property_flags_from_dwFileAttributes(
    DWORD dwFileAttributes
) {
    FilePropertyFlags flags = 0;
    if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        flags |= FilePropertyFlag_IsFolder;
    }
    return flags;
}

internal void os_w32_dense_time_from_file_time(DenseTime *out, FILETIME *in)
{
    SYSTEMTIME systime = {0};
    FileTimeToSystemTime(in, &systime);
    DateTime date_time = {0};
    os_w32_date_time_from_system_time(&date_time, &systime);
    *out = dense_time_from_date_time(date_time);
}

internal void os_w32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in)
{
    out->year    = in->wYear;
    out->mon     = in->wMonth - 1;
    out->wday    = in->wDayOfWeek;
        out->day     = in->wDay;
        out->hour    = in->wHour;
        out->min     = in->wMinute;
        out->sec     = in->wSecond;
    out->msec    = in->wMilliseconds;
}

// Memory Allocation
//=============================================================================

internal void *os_memory_create(U64 size)
{
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal bool os_memory_commit(void *ptr, U64 size)
{
    bool result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

internal void os_memory_decommit(void *ptr, U64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void os_memory_free(void *ptr, U64 size)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}

// File System
//=============================================================================

internal Os_File os_file_open(Str8 path, Os_AccessFlags flags)
{
    Str16 path16 = str16_from_8(os_core_state.alloc, path);
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;
    SECURITY_ATTRIBUTES security_attributes = {sizeof(security_attributes), 0, 0};
    if(flags & OS_AccessFlag_Read) {
        access_flags |= GENERIC_READ;
    }
    if(flags & OS_AccessFlag_Write) {
        access_flags |= GENERIC_WRITE;
    }
    if(flags & OS_AccessFlag_Execute) {
        access_flags |= GENERIC_EXECUTE;
    }
    if(flags & OS_AccessFlag_ShareRead) {
        share_mode |= FILE_SHARE_READ;
    }
    if(flags & OS_AccessFlag_ShareWrite) {
        share_mode |= FILE_SHARE_WRITE|FILE_SHARE_DELETE;
    }
    if(flags & OS_AccessFlag_Write) {
        creation_disposition = CREATE_ALWAYS;
    }
    if(flags & OS_AccessFlag_Append) {
        creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA;
    }
    if(flags & OS_AccessFlag_Inherited)
    {
        security_attributes.bInheritHandle = 1;
    }
    HANDLE handle = CreateFileW(
        (WCHAR *)path16.str, access_flags, share_mode, &security_attributes,
        creation_disposition, FILE_ATTRIBUTE_NORMAL, 0
    );
    return (Os_File)handle;
}

internal void os_file_close(Os_File file)
{
    CloseHandle((HANDLE)file);
}

internal U64 os_file_read(Os_File file, Rng1U64 rng, void *out_data)
{
    U64 size = 0;
    GetFileSizeEx((HANDLE)file, (LARGE_INTEGER *)&size);
    Rng1U64 rng_clamped  = rng_1u64(Min(rng.min, size), Min(rng.max, size));
    U64 total_read_size = 0;

    // read loop
    {
        U64 to_read = dim_1u64(rng_clamped);
        for(U64 off = rng.min; total_read_size < to_read;)
        {
            U64 amt64 = to_read - total_read_size;
            U32 amt32 = u32_from_u64_saturate(amt64);
            DWORD read_size = 0;
            OVERLAPPED overlapped = {0};
            overlapped.Offset     = (off&0x00000000ffffffffull);
            overlapped.OffsetHigh = (off&0xffffffff00000000ull) >> 32;
            ReadFile((HANDLE)file, (U8 *)out_data + total_read_size, amt32, &read_size, &overlapped);
            off += read_size;
            total_read_size += read_size;
                if(read_size != amt32)
                {
                    break;
                }
        }
    }

    return total_read_size;
}

internal U64 os_file_write(Os_File file, Rng1U64 rng, void *data)
{
    U64 src_off = 0;
    U64 dst_off = rng.min;
    U64 total_write_size = dim_1u64(rng);
    for(;;)
    {
        void *bytes_src = (U8 *)data + src_off;
        U64 bytes_left = total_write_size - src_off;
        DWORD write_size = Min(MB(1), bytes_left);
        DWORD bytes_written = 0;
        OVERLAPPED overlapped = {0};
        overlapped.Offset = (dst_off&0x00000000ffffffffull);
        overlapped.OffsetHigh = (dst_off&0xffffffff00000000ull) >> 32;
        BOOL success = WriteFile((HANDLE)file, bytes_src, write_size, &bytes_written, &overlapped);
        if(success == 0)
        {
            break;
        }
        src_off += bytes_written;
        dst_off += bytes_written;
        if(bytes_left == 0)
        {
            break;
        }
    }
    return src_off;
}

internal Os_FileProperties os_file_properties(Os_File file)
{
    Os_FileProperties props = {0};
    BY_HANDLE_FILE_INFORMATION info;
    BOOL info_good = GetFileInformationByHandle((HANDLE)file, &info);
    if(info_good)
    {
        U32 size_lo = info.nFileSizeLow;
        U32 size_hi = info.nFileSizeHigh;
        props.size  = (U64)size_lo | (((U64)size_hi)<<32);
        os_w32_dense_time_from_file_time(&props.modified, &info.ftLastWriteTime);
        os_w32_dense_time_from_file_time(&props.created, &info.ftCreationTime);
        props.flags = os_w32_file_property_flags_from_dwFileAttributes(info.dwFileAttributes);
    }
    return props;
}

internal bool os_dir_make(Str8 path)
{
    bool result = false;
    Str16 path16 = str16_from_8(os_core_state.alloc, path);
    WIN32_FILE_ATTRIBUTE_DATA attributes = {0};
    GetFileAttributesExW((WCHAR*)path16.str, GetFileExInfoStandard, &attributes);
    if(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        result = true;
    } else if(CreateDirectoryW((WCHAR*)path16.str, 0)) {
        result = true;
    }
    return(result);
}

// Exit
//=============================================================================

internal void os_exit(I32 exit_code)
{
    ExitProcess(exit_code);
}

// Time
//=============================================================================

internal U32 os_now_unix(void)
{
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    U32 unix_time = os_w32_unix_time_from_file_time(file_time);
    return unix_time;
}

internal U64 os_now_microsec(void)
{
    U64 result = 0;
    LARGE_INTEGER large_int_counter;
    if(QueryPerformanceCounter(&large_int_counter))
    {
        result = (large_int_counter.QuadPart*Million(1))/os_w32_state.microsecond_resolution;
    }
    return result;
}

internal void os_sleep_microsec(U64 microsec)
{
    DWORD millisec = (DWORD)(microsec / 1000); // Integer division to get milliseconds
    if (microsec % 1000 >= 500) {
        millisec++; // Round up if the remaining microseconds are 500 or more
    }
    os_sleep_millisec(millisec);
}

internal void os_sleep_millisec(U32 millisec)
{
    Sleep(millisec);
}

// OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    os_w32_state.microsecond_resolution  = 1;
    os_entry_point(argc, argv);
}
