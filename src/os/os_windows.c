// Helpers Functions
//=============================================================================

internal uint32_t _os_win32_unix_time_from_file_time(FILETIME file_time)
{
    uint64_t win32_time = ((uint64_t)file_time.dwHighDateTime << 32) | file_time.dwLowDateTime;
    uint64_t unix_time64 = ((win32_time - 0x19DB1DED53E8000ULL) / 10000000);

    Assert(unix_time64 <= max_u32);
    uint32_t unix_time32 = (uint32_t)unix_time64;

    return unix_time32;
}

internal FilePropertyFlags _os_win32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes) {
    FilePropertyFlags flags = 0;
    if(dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        flags |= FilePropertyFlag_IsFolder;
    }
    return flags;
}

internal void _os_win32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in)
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

internal void _os_win32_dense_time_from_file_time(DenseTime *out, FILETIME *in)
{
    SYSTEMTIME systime = {0};
    FileTimeToSystemTime(in, &systime);
    DateTime date_time = {0};
    _os_win32_date_time_from_system_time(&date_time, &systime);
    *out = dense_time_from_date_time(date_time);
}

// Memory Allocation
//=============================================================================

internal void *os_memory_create(uint64_t size)
{
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal bool os_memory_commit(void *ptr, uint64_t size)
{
    bool result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return result;
}

internal void os_memory_decommit(void *ptr, uint64_t size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal void os_memory_free(void *ptr, uint64_t size)
{
    Unused(size);
    VirtualFree(ptr, 0, MEM_RELEASE);
}

// File System
//=============================================================================

internal Os_File os_file_open(Str8 path, Os_AccessFlags flags)
{
    Str16 path16 = str16_from_8(_os_core_state.alloc, path);
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
        (WCHAR *)path16.cstr, access_flags, share_mode, &security_attributes,
        creation_disposition, FILE_ATTRIBUTE_NORMAL, 0
    );
    return (Os_File)handle;
}

internal void os_file_close(Os_File file)
{
    CloseHandle((HANDLE)file);
}

internal uint64_t os_file_read(Os_File file, Rng1_U64 rng, void *out_data)
{
    uint64_t size = 0;
    GetFileSizeEx((HANDLE)file, (LARGE_INTEGER *)&size);
    Rng1_U64 rng_clamped  = rng1_u64(Min(rng.min, size), Min(rng.max, size));
    uint64_t total_read_size = 0;
    // read loop
    {
        uint64_t to_read = dim1_u64(rng_clamped);
        for(uint64_t off = rng.min; total_read_size < to_read;)
        {
            uint64_t amt64 = to_read - total_read_size;
            uint32_t amt32 = u32_from_u64_saturate(amt64);
            DWORD read_size = 0;
            OVERLAPPED overlapped = {0};
            overlapped.Offset     = (off&0x00000000ffffffffull);
            overlapped.OffsetHigh = (off&0xffffffff00000000ull) >> 32;
            ReadFile((HANDLE)file, (uint8_t *)out_data + total_read_size, amt32, &read_size, &overlapped);
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

internal uint64_t os_file_write(Os_File file, Rng1_U64 rng, void *data)
{
    uint64_t src_off = 0;
    uint64_t dst_off = rng.min;
    uint64_t total_write_size = dim1_u64(rng);
    for(;;)
    {
        void *bytes_src = (uint8_t *)data + src_off;
        uint64_t bytes_left = total_write_size - src_off;
        DWORD write_size = (DWORD)Min(MB(1), bytes_left);
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
        uint32_t size_lo = info.nFileSizeLow;
        uint32_t size_hi = info.nFileSizeHigh;
        props.size  = (uint64_t)size_lo | (((uint64_t)size_hi)<<32);
        _os_win32_dense_time_from_file_time(&props.modified, &info.ftLastWriteTime);
        _os_win32_dense_time_from_file_time(&props.created, &info.ftCreationTime);
        props.flags = _os_win32_file_property_flags_from_dwFileAttributes(info.dwFileAttributes);
    }
    return props;
}

internal bool os_dir_make(Str8 path)
{
    bool result = false;
    Str16 path16 = str16_from_8(_os_core_state.alloc, path);
    WIN32_FILE_ATTRIBUTE_DATA attributes = {0};
    GetFileAttributesExW((WCHAR*)path16.cstr, GetFileExInfoStandard, &attributes);
    if(attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        result = false;
    } else if(CreateDirectoryW((WCHAR*)path16.cstr, 0)) {
        result = true;
    }
    return(result);
}

// Exit
//=============================================================================

internal void os_exit(int32_t exit_code)
{
    ExitProcess(exit_code);
}

// Time
//=============================================================================

internal uint32_t os_now_unix(void)
{
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    uint32_t unix_time = _os_win32_unix_time_from_file_time(file_time);
    return unix_time;
}

internal uint64_t os_now_microsec(void)
{
    uint64_t result = 0;
    LARGE_INTEGER large_int_counter;
    if(QueryPerformanceCounter(&large_int_counter))
    {
        result = (large_int_counter.QuadPart*Million(1))/_os_win32_state.microsecond_resolution;
    }
    return result;
}

internal void os_sleep_microsec(uint64_t microsec)
{
    DWORD millisec = (DWORD)(microsec / 1000); // Integer division to get milliseconds
    if (microsec % 1000 >= 500) {
        millisec++; // Round up if the remaining microseconds are 500 or more
    }
    os_sleep_millisec(millisec);
}

internal void os_sleep_millisec(uint32_t millisec)
{
    Sleep(millisec);
}

// Environment Variable
// ============================================================================

internal bool os_env_is_set(Str8 name)
{
    Str16 name16 = str16_from_8(_os_core_state.alloc, name);
    DWORD len = GetEnvironmentVariableW(name16.cstr, NULL, 0);
    return len;
}

internal Str8 os_env_get(Str8 name)
{
    Str8 result = ZERO_STRUCT;
    Str16 name16 = str16_from_8(_os_core_state.alloc, name);
    DWORD len = GetEnvironmentVariableW(name16.cstr, NULL, 0);
    if (len > 0) {
        uint16_t* value_buf = alloc_make(_os_core_state.alloc, uint16_t, len);
        GetEnvironmentVariableW(name16.cstr, value_buf, len);
        Str16 value16 = str16_from_cstr(value_buf);
        alloc_free(_os_core_state.alloc, value_buf, len);
        result = str8_from_16(_os_core_state.alloc, value16);
    }
    return result;
}

// OS Entry Points
//=============================================================================

int main(void)
{
    // Allocating core memory
    uint64_t size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    // Setup argument array
    int args_count;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &args_count);
    _os_core_state.args = str8_array_alloc(alloc, args_count);
    _os_core_state.alloc = alloc;
    for(int i = 0; i < args_count; i++)
    {
        Str16 str16 = str16_from_cstr(args[i]);
        Str8 str = str8_from_16(alloc, str16);
        str8_array_append(&_os_core_state.args, str);
    }
    // NOTE(AnzenKodo): we need this to set now time.
    {
        _os_win32_state.microsecond_resolution  = 1;
        LARGE_INTEGER large_int_resolution;
        if(QueryPerformanceFrequency(&large_int_resolution))
        {
            _os_win32_state.microsecond_resolution = large_int_resolution.QuadPart;
        }
    }
    // Go to default OS entry point
    os_entry_point();
}
