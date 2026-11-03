//-----------------------------------------------------------------------------
// Reference:
// - By Epic Games Tools & Ryan Fleury: https://github.com/EpicGames/raddebugger/tree/e28a543573277315ffd32a3e983c8171570bb946/src/win32/base
// - By Allen Webster: https://github.com/4coder-archive/4coder/tree/master/platform_win32
//-----------------------------------------------------------------------------

//~ ak: External Includes
//=============================================================================
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
// #define NOVIRTUALKEYCODES
// #define NOSYSMETRICS
// #define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
// #define NORASTEROPS
// #define NOSHOWWINDOW
#define NOATOM
#define NOMETAFILE
// #define NOMSG
#define NOSCROLL
#define NOTEXTMETRIC
// #define NOWINOFFSETS
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOCRYPT
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#include <winsock2.h>
#include <winuser.h>
#include <mswsock.h>
#include <shellapi.h>
#include <winuser.h>
#include <shellscalingapi.h>
#include <sysinfoapi.h>
#include <shlobj.h>

//~ ak: Private Types
//=============================================================================

typedef struct _Win32_State _Win32_State;
struct _Win32_State
{
    Arena *arena;
    uint64_t microsecond_resolution;
    SYSTEM_INFO system_info;
};

typedef struct _Win32_Walk_Iter _Win32_Walk_Iter;
struct _Win32_Walk_Iter
{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
    bool is_volume_iter;
    Str8_Array drive_strings;
    uint64_t drive_strings_iter_idx;
};

//~ ak: Function Pointers
//=============================================================================

typedef BOOL w32_SetProcessDpiAwarenessContext_Type(void* value);
typedef UINT w32_GetDpiForWindow_Type(HWND hwnd);
typedef HRESULT w32_GetDpiForMonitor_Type(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);
typedef int w32_GetSystemMetricsForDpi_Type(int nIndex, UINT dpi);
#define w32_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((void*)-4)
global w32_GetDpiForWindow_Type *w32_GetDpiForWindow_func = 0;
global w32_GetDpiForMonitor_Type *w32_GetDpiForMonitor_func = 0;
global w32_GetSystemMetricsForDpi_Type *w32_GetSystemMetricsForDpi_func = 0;

//~ ak: Private Global Variables
//=============================================================================

global _Win32_State _win32_state = STRUCT_ZERO;

//~ ak: Private Function Definition
//=============================================================================

//- ak: Helper ================================================================

internal uint32_t _win32_unix_time_from_file_time(FILETIME file_time);
internal Fs_File_Property_Flags _win32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);
internal void _win32_dense_time_from_file_time(DenseTime *out, FILETIME *in);
internal void _win32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in);
internal uint32_t _win32_sleep_ms_from_endt_us(uint64_t endt_us);

//~ ak: Private Function Implementation
//=============================================================================

//- ak: Helper ================================================================

internal uint32_t _win32_unix_time_from_file_time(FILETIME file_time)
{
    uint64_t win32_time = ((uint64_t)file_time.dwHighDateTime << 32) | file_time.dwLowDateTime;
    uint64_t unix_time64 = ((win32_time - 0x19DB1DED53E8000ULL) / 10000000);

    Assert(unix_time64 <= max_u32);
    uint32_t unix_time32 = (uint32_t)unix_time64;
    return unix_time32;
}

internal Fs_File_Property_Flags _win32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes)
{
    Fs_File_Property_Flags flags = 0;
    if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        flags |= Fs_File_Property_Flag_IsFolder;
    }
    return flags;
}

internal void _win32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in)
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

internal void _win32_dense_time_from_file_time(DenseTime *out, FILETIME *in)
{
    SYSTEMTIME systime = STRUCT_ZERO;
    FileTimeToSystemTime(in, &systime);
    DateTime date_time = STRUCT_ZERO;
    _win32_date_time_from_system_time(&date_time, &systime);
    *out = dense_time_from_date_time(date_time);
}

internal uint32_t _win32_sleep_ms_from_endt_us(uint64_t endt_us)
{
    uint32_t sleep_ms = 0;
    if(endt_us == max_u64)
    {
        sleep_ms = INFINITE;
    }
    else
    {
        uint64_t begint = time_now_us();
        if(begint < endt_us)
        {
            uint64_t sleep_us = endt_us - begint;
            sleep_ms = (uint32_t)((sleep_us + 999)/1000);
        }
    }
    return sleep_ms;
}

//~ ak: Os Functions for Base Layer
//=============================================================================

//- ak: Memory =================================================================

internal void *mem_reserve(size_t size)
{
    void *result = VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
    return result;
}

internal bool mem_commit(void *ptr, size_t size)
{
    LPVOID result = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return result != NULL;
}

internal bool mem_decommit(void *ptr, size_t size)
{
    return VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal bool mem_release(void *ptr, size_t size)
{
    return VirtualFree(ptr, size, MEM_RELEASE);
}

// ak: File System ============================================================

/// ak: File Operations

internal Fs_File fs_file_open(Str8 path, Fs_Access_Flags flags)
{
    Fs_File result = STRUCT_ZERO;
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    DWORD access_flags = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = OPEN_EXISTING;
    SECURITY_ATTRIBUTES security_attributes = {sizeof(security_attributes), 0, 0};
    if(flags & Fs_Access_Flag_Read)        {access_flags |= GENERIC_READ;}
    if(flags & Fs_Access_Flag_Write)       {access_flags |= GENERIC_WRITE;}
    if(flags & Fs_Access_Flag_Execute)     {access_flags |= GENERIC_EXECUTE;}
    if(flags & Fs_Access_Flag_ShareRead)   {share_mode |= FILE_SHARE_READ;}
    if(flags & Fs_Access_Flag_ShareWrite)  {share_mode |= FILE_SHARE_WRITE|FILE_SHARE_DELETE;}
    if(flags & Fs_Access_Flag_Write)       {creation_disposition = CREATE_ALWAYS;}
    if(flags & Fs_Access_Flag_Append)      {creation_disposition = OPEN_ALWAYS; access_flags |= FILE_APPEND_DATA; }
    if(flags & Fs_Access_Flag_CreateNew)   {creation_disposition = CREATE_NEW;}
    if(flags & Fs_Access_Flag_Inherited)
    {
        security_attributes.bInheritHandle = 1;
    }
    HANDLE file = CreateFileW((WCHAR *)path16.cstr, access_flags, share_mode, &security_attributes, creation_disposition, FILE_ATTRIBUTE_NORMAL, 0);
    if(file != INVALID_HANDLE_VALUE)
    {
        result.id[0] = (uint64_t)file;
    }
    else
    {
        DWORD err = GetLastError();
        (void)err;
    }
    arena_scratch_end(scratch);
    return result;
}

internal bool file_flush(Fs_File file)
{
    return FlushFileBuffers(HandleUnpack(HANDLE, file));
}

internal void fs_file_close(Fs_File file)
{
    if (fs_file_is_zero(file)) { return; }
    HANDLE handle = HandleUnpack(HANDLE, file);
    BOOL result = CloseHandle(handle);
    (void)result;
}

internal uint64_t fs_file_read(Fs_File file, Rng1_U64 rng, void *out_data)
{
    if (fs_file_is_zero(file)) { return 0; }
    
    HANDLE handle = HandleUnpack(HANDLE, file);
    uint8_t *ptr = out_data;
    uint64_t off = rng.min;
    while(off != rng.max)
    {
        uint64_t amt64 = rng.max - off;
        uint32_t amt32 = (uint32_t)Min(MB(32), amt64);
        DWORD read_size  = 0;
        OVERLAPPED overlapped = {
            .Offset = (uint32_t)off, .OffsetHigh = (uint32_t)(off >> 32)
        };
        if( ! ReadFile(handle, ptr, amt32, &read_size, &overlapped))
        {
            break;
        }
        ptr += read_size;
        off += read_size;
    }
    
    uint64_t total_read_size = off - rng.min;
    return total_read_size;
}


internal size_t fs_file_write(Fs_File file, void *data, Rng1_U64 rng)
{
    if (fs_file_is_zero(file)) { return 0; }
    HANDLE handle = HandleUnpack(HANDLE, file);
    uint64_t src_off = 0;
    uint64_t dst_off = rng.min;
    uint64_t total_write_size = dim_rng1(rng);
    for(;;)
    {
        void *bytes_src = (uint8_t *)data + src_off;
        uint64_t bytes_left = total_write_size - src_off;
        DWORD write_size = Min(MB(1), bytes_left);
        DWORD bytes_written = 0;
        OVERLAPPED overlapped = STRUCT_ZERO;
        overlapped.Offset = (dst_off&0x00000000ffffffffull);
        overlapped.OffsetHigh = (dst_off&0xffffffff00000000ull) >> 32;
        BOOL success = WriteFile(handle, bytes_src, write_size, &bytes_written, &overlapped);
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

internal Fs_File_Properties fs_file_properties_from_file(Fs_File file)
{
    if (fs_file_is_zero(file)) { return StructZeroType(Fs_File_Properties); }
    Fs_File_Properties props = STRUCT_ZERO;
    HANDLE handle = HandleUnpack(HANDLE, file);
    BY_HANDLE_FILE_INFORMATION info;
    BOOL info_good = GetFileInformationByHandle(handle, &info);
    if(info_good)
    {
        uint32_t size_lo = info.nFileSizeLow;
        uint32_t size_hi = info.nFileSizeHigh;
        props.size = (size_t)size_lo | (((size_t)size_hi)<<32);
        _win32_dense_time_from_file_time(&props.modified, &info.ftLastWriteTime);
        _win32_dense_time_from_file_time(&props.created, &info.ftCreationTime);
        props.flags = _win32_file_property_flags_from_dwFileAttributes(info.dwFileAttributes);
    }
    return props;
}

/// ak: Path Operations

internal bool fs_file_path_exists(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    DWORD attributes = GetFileAttributesW((WCHAR *)path16.cstr);
    bool exists = (attributes != INVALID_FILE_ATTRIBUTES) && !!(~attributes & FILE_ATTRIBUTE_DIRECTORY);
    arena_scratch_end(scratch);
    return exists;
}

internal bool fs_file_path_move(Str8 dst, Str8 src)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 dst16 = str16_from_8(scratch.arena, dst);
    Str16 src16 = str16_from_8(scratch.arena, src);
    bool result = MoveFileW((WCHAR*)src16.cstr, (WCHAR*)dst16.cstr);
    arena_scratch_end(scratch);
    return result;
}

internal bool fs_file_path_copy(Str8 dst, Str8 src)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 dst16 = str16_from_8(scratch.arena, dst);
    Str16 src16 = str16_from_8(scratch.arena, src);
    bool result = CopyFileW((WCHAR*)src16.cstr, (WCHAR*)dst16.cstr, 0);
    arena_scratch_end(scratch);
    return result;
}

internal bool fs_file_path_replace(Str8 dst, Str8 src)
{
    Arena_Temp scratch = arena_scratch_begin(NULL,0);
    Str16 dst16 = str16_from_8(scratch.arena, dst);
    Str16 src16 = str16_from_8(scratch.arena, src);
    bool result = MoveFileExW((WCHAR *)src16.cstr, (WCHAR *)dst16.cstr, MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
    arena_scratch_end(scratch);
    return result;
}

internal bool fs_file_path_delete(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    bool result = DeleteFileW((WCHAR*)path16.cstr);
    arena_scratch_end(scratch);
    return result;
}

internal Str8 fs_full_path_from_path(Arena *arena, Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(&arena, 1);
    DWORD buffer_size = Max(MAX_PATH, path.size * 2) + 1;
    Str16 path16 = str16_from_8(scratch.arena, path);
    WCHAR *buffer = arena_push_nz(scratch.arena, WCHAR, buffer_size);
    DWORD path16_size = GetFullPathNameW((WCHAR*)path16.cstr, buffer_size, buffer, NULL);
    if(path16_size > buffer_size)
    {
        arena_pop(scratch.arena, buffer_size);
        buffer_size = path16_size + 1;
        buffer = arena_push_nz(scratch.arena, WCHAR, buffer_size);
        path16_size = GetFullPathNameW((WCHAR*)path16.cstr, buffer_size, buffer, NULL);
    }
    Str8 full_path = str8_from_16(arena, str16_init((uint16_t*)buffer, path16_size));
    arena_scratch_end(scratch);
    return full_path;
}

internal Fs_File_Properties fs_file_properties_from_file_path(Str8 path)
{
    WIN32_FIND_DATAW find_data = STRUCT_ZERO;
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    HANDLE handle = FindFirstFileW((WCHAR *)path16.cstr, &find_data);
    Fs_File_Properties props = STRUCT_ZERO;
    if(handle != INVALID_HANDLE_VALUE)
    {
        props.size = Compose64Bit(find_data.nFileSizeHigh, find_data.nFileSizeLow);
        _win32_dense_time_from_file_time(&props.created, &find_data.ftCreationTime);
        _win32_dense_time_from_file_time(&props.modified, &find_data.ftLastWriteTime);
        props.flags = _win32_file_property_flags_from_dwFileAttributes(find_data.dwFileAttributes);
    }
    else
    {
        Arena_Temp scratch = arena_scratch_begin(NULL, 0);
        WCHAR buffer[512] = STRUCT_ZERO;
        DWORD length = GetLogicalDriveStringsW(sizeof(buffer), buffer);
        size_t last_slash_pos = 0;
        for(;last_slash_pos < path.size; last_slash_pos = str8_find_substr(path, last_slash_pos+1, str8("/"), Str_Match_Flag_SlashInsensitive));
        Str8 path_trimmed = str8_prefix(path, last_slash_pos);
        for(size_t off = 0; off < (size_t)length;)
        {
            Str16 next_drive_string_16 = str16_from_cstr((uint16_t *)buffer+off);
            off += next_drive_string_16.size+1;
            Str8 next_drive_string = str8_from_16(scratch.arena, next_drive_string_16);
            next_drive_string = str8_chop_last_slash(next_drive_string);
            if(str8_match(path_trimmed, next_drive_string, Str_Match_Flag_CaseInsensitive))
            {
                props.flags |= Fs_File_Property_Flag_IsFolder;
                break;
            }
        }
        arena_scratch_end(scratch);
    }
    FindClose(handle);
    arena_scratch_end(scratch);
    return props;
}

/// ak: Directory Operations

internal bool fs_dir_exists(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    DWORD attr = GetFileAttributesW((WCHAR*)path16.cstr);
    arena_scratch_end(scratch);
    return (attr != INVALID_FILE_ATTRIBUTES) && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

internal bool fs_dir_make(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str16 path16 = str16_from_8(scratch.arena, path);
    BOOL result = CreateDirectoryW((WCHAR*)path16.cstr, NULL);
    arena_scratch_end(scratch);
    return result != 0;
}

/// ak: File System Walk

internal Fs_Walk *fs_file_walk_begin(Arena *arena, Str8 path, Fs_Walk_Flags flags)
{
    Arena_Temp scratch = arena_scratch_begin(&arena, 1);
    Str8 path_with_wildcard = str8_cat(scratch.arena, path, str8("\\*"));
    Str16 path16 = str16_from_8(scratch.arena, path_with_wildcard);
    Fs_Walk *walk = arena_push(arena, Fs_Walk, 1);
    walk->flags = flags;
    _Win32_Walk_Iter *win32_walk = (_Win32_Walk_Iter*)walk->memory;
    if (path.size == 0)
    {
        win32_walk->is_volume_iter = 1;
        WCHAR buffer[512] = STRUCT_ZERO;
        DWORD length = GetLogicalDriveStringsW(sizeof(buffer), buffer);
        Str8_List drive_strings = STRUCT_ZERO;
        for (uint64_t off = 0; off < (uint64_t)length;)
        {
            Str16 next_drive_string_16 = str16_from_cstr((uint16_t *)buffer+off);
            off += next_drive_string_16.size+1;
            Str8 next_drive_string = str8_from_16(arena, next_drive_string_16);
            next_drive_string = str8_chop_last_slash(next_drive_string);
            str8_list_push(scratch.arena, &drive_strings, next_drive_string);
        }
        win32_walk->drive_strings = str8_array_from_list(arena, &drive_strings);
        win32_walk->drive_strings_iter_idx = 0;
    }
    else
    {
        win32_walk->handle = FindFirstFileExW((WCHAR*)path16.cstr, FindExInfoBasic, &win32_walk->find_data, FindExSearchNameMatch, 0, FIND_FIRST_EX_LARGE_FETCH);
    }
    arena_scratch_end(scratch);
    return walk;
}

internal bool fs_file_walk_next(Arena *arena, Fs_Walk *walk, Fs_Info *info_out)
{
    bool result = 0;
    Fs_Walk_Flags flags = walk->flags;
    _Win32_Walk_Iter *win32_walk = (_Win32_Walk_Iter*)walk->memory;
    if (win32_walk->is_volume_iter)
    {
        //- ak: volume iteration
        result = win32_walk->drive_strings_iter_idx < win32_walk->drive_strings.length;
        if (result != 0)
        {
            MemSetZeroStruct(info_out);
            info_out->name = win32_walk->drive_strings.v[win32_walk->drive_strings_iter_idx];
            info_out->props.flags |= Fs_File_Property_Flag_IsFolder;
            win32_walk->drive_strings_iter_idx += 1;
        }
    }
    else
    {
        //- ak: file iteration
        if (!(flags & Fs_Walk_Flag_Done) && win32_walk->handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                //- ak: check is usable
                bool usable_file = 1;
                WCHAR *file_name = win32_walk->find_data.cFileName;
                DWORD attributes = win32_walk->find_data.dwFileAttributes;
                if (file_name[0] == '.')
                {
                    if (flags & Fs_Walk_Flag_SkipHiddenFiles)
                    {
                        usable_file = 0;
                    }
                    else if (file_name[1] == 0)
                    {
                        usable_file = 0;
                    }
                    else if (file_name[1] == '.' && file_name[2] == 0)
                    {
                        usable_file = 0;
                    }
                }
                if (attributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (flags & Fs_Walk_Flag_SkipFolders)
                    {
                        usable_file = 0;
                    }
                }
                else{
                    if (flags & Fs_Walk_Flag_SkipFiles)
                    {
                        usable_file = 0;
                    }
                }
                //- ak: emit if usable
                if (usable_file)
                {
                    info_out->name = str8_from_16(arena, str16_from_cstr((uint16_t*)file_name));
                    info_out->props.size = (uint64_t)win32_walk->find_data.nFileSizeLow | (((uint64_t)win32_walk->find_data.nFileSizeHigh)<<32);
                    _win32_dense_time_from_file_time(&info_out->props.created,  &win32_walk->find_data.ftCreationTime);
                    _win32_dense_time_from_file_time(&info_out->props.modified, &win32_walk->find_data.ftLastWriteTime);
                    info_out->props.flags = _win32_file_property_flags_from_dwFileAttributes(attributes);
                    result = 1;
                    if (!FindNextFileW(win32_walk->handle, &win32_walk->find_data))
                    {
                        walk->flags |= Fs_Walk_Flag_Done;
                    }
                    break;
                }
            }while(FindNextFileW(win32_walk->handle, &win32_walk->find_data));
        }
    }
    if (!result)
    {
        walk->flags |= Fs_Walk_Flag_Done;
    }
    return result;
}

internal void fs_file_walk_end(Fs_Walk *walk)
{
    _Win32_Walk_Iter *win32_walk = (_Win32_Walk_Iter*)walk->memory;
    HANDLE zero_handle;
    MemSetZeroStruct(&zero_handle);
    if (!MemMatchStruct(&zero_handle, &win32_walk->handle))
    {
        FindClose(win32_walk->handle);
    }
}

//- ak: Time ===================================================================

internal uint32_t time_now_unix(void)
{
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    uint32_t unix_time = _win32_unix_time_from_file_time(file_time);
    return unix_time;
}

internal uint64_t time_now_us(void)
{
    uint64_t result = 0;
    LARGE_INTEGER large_int_counter;
    if (QueryPerformanceCounter(&large_int_counter))
    {
        result = (large_int_counter.QuadPart*Million(1))/_win32_state.microsecond_resolution;
    }
    return result;
}

//- ak: Processes =============================================================

/// ak: Process Management

internal void process_sleep_us(uint64_t microsec)
{
    DWORD millisec = (DWORD)(microsec / 1000); // integer division to get milliseconds
    if (microsec % 1000 >= 500)
    {
        millisec++; // round up if the remaining microseconds are 500 or more
    }
    process_sleep_ms(millisec);
}

internal void process_sleep_ms(uint32_t millisec)
{
    Sleep(millisec);
}

internal void process_exit(int32_t exit_code)
{
    ExitProcess(exit_code);
}

/// ak: Process Info

internal Str8 process_path_get(Arena *arena)
{
    Arena_Temp scratch = arena_scratch_begin(&arena, 1);
    DWORD length = GetCurrentDirectoryW(0, 0);
    uint16_t *memory = arena_push_nz(scratch.arena, uint16_t, length + 1);
    length = GetCurrentDirectoryW(length + 1, (WCHAR*)memory);
    Str8 name = str8_from_16(arena, str16_init(memory, length));
    arena_scratch_end(scratch);
    return name;
}

internal uint32_t process_start_time_unix_get(void)
{
    uint32_t result = 0;
    HANDLE handle = GetCurrentProcess();
    FILETIME start_time = STRUCT_ZERO;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    if(GetProcessTimes(handle, &start_time, &exit_time, &kernel_time, &user_time))
    {
        result = _win32_unix_time_from_file_time(start_time);
    }
    return result;
}

/// ak: Child Processes

internal Process process_init(Process_Launch_Params *params)
{
    Process result = STRUCT_ZERO;
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    
    // ak: form full command string
    Str8 cmd = STRUCT_ZERO;
    {
        Str_Join join_params = STRUCT_ZERO;
        join_params.pre = str8("\"");
        join_params.sep = str8("\" \"");
        join_params.post = str8("\"");
        cmd = str8_list_join(scratch.arena, &params->cmd_line, &join_params);
    }
    
    // ak: form environment
    bool use_null_env_arg = 0;
    Str8 env = STRUCT_ZERO;
    {
        Str_Join join_params2 = STRUCT_ZERO;
        join_params2.sep = str8("\0");
        join_params2.post = str8("\0");
        Str8_List all_opts = params->env;
        if(params->inherit_env != 0)
        {
            if(all_opts.length != 0)
            {
                MemSetZeroStruct(&all_opts);
                for(Str8_Node *n = params->env.first; n != 0; n = n->next)
                {
                    str8_list_push(scratch.arena, &all_opts, n->str);
                }
                for(Str8_Node *n = _os_state.process_info.environment.first; n != 0; n = n->next)
                {
                    str8_list_push(scratch.arena, &all_opts, n->str);
                }
            }
            else
            {
                use_null_env_arg = 1;
            }
        }
        if(use_null_env_arg == 0)
        {
            env = str8_list_join(scratch.arena, &all_opts, &join_params2);
        }
    }
    
    // ak: utf-8 -> utf-16
    Str16 cmd16 = str16_from_8(scratch.arena, cmd);
    Str16 dir16 = str16_from_8(scratch.arena, params->path);
    Str16 env16 = STRUCT_ZERO;
    if(use_null_env_arg == 0)
    {
        env16 = str16_from_8(scratch.arena, env);
    }
    
    // ak: determine creation flags
    DWORD creation_flags = CREATE_UNICODE_ENVIRONMENT;
    if(params->consoleless)
    {
        creation_flags |= CREATE_NO_WINDOW;
    }
    if(params->new_console)
    {
        creation_flags |= CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
    }
    if(params->process_group.id[0] != 0)
    {
        creation_flags |= CREATE_SUSPENDED;
    }
    
    // ak: launch
    BOOL inherit_handles = 0;
    STARTUPINFOW startup_info = STRUCT_ZERO;
    startup_info.cb = sizeof(startup_info);
    if(!fs_file_is_zero(params->stdout_file))
    {
        HANDLE stdout_handle = (HANDLE)params->stdout_file.id[0];
        startup_info.hStdOutput = stdout_handle;
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
        inherit_handles = 1;
    }
    if(!fs_file_is_zero(params->stderr_file))
    {
        HANDLE stderr_handle = (HANDLE)params->stderr_file.id[0];
        startup_info.hStdError = stderr_handle;
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
        inherit_handles = 1;
    }
    if(!fs_file_is_zero(params->stdin_file))
    {
        HANDLE stdin_handle = (HANDLE)params->stdin_file.id[0];
        startup_info.hStdInput = stdin_handle;
        startup_info.dwFlags |= STARTF_USESTDHANDLES;
        inherit_handles = 1;
    }
    PROCESS_INFORMATION process_info = STRUCT_ZERO;
    if(CreateProcessW(0, (WCHAR*)cmd16.cstr, 0, 0, inherit_handles, creation_flags, use_null_env_arg ? 0 : (WCHAR*)env16.cstr, (WCHAR*)dir16.cstr, &startup_info, &process_info))
    {
        bool group_is_ok = params->process_group.id[0] == 0 ||
            AssignProcessToJobObject((HANDLE)params->process_group.id[0], process_info.hProcess);
        bool resume_is_ok = params->process_group.id[0] == 0 || ResumeThread(process_info.hThread) != (DWORD)-1;
        if(group_is_ok && resume_is_ok)
        {
            result.id[0] = (uint64_t)process_info.hProcess;
        }
        else
        {
            TerminateProcess(process_info.hProcess, 999);
            WaitForSingleObject(process_info.hProcess, INFINITE);
            CloseHandle(process_info.hProcess);
        }
        CloseHandle(process_info.hThread);
    }
    
    arena_scratch_end(scratch);
    return result;
}

internal uint64_t pid_from_process(Process process)
{
    HANDLE process_handle = (HANDLE)process.id[0];
    uint64_t result = (uint64_t)GetProcessId(process_handle);
    return result;
}

internal bool process_poll(Process process, uint64_t *exit_code_out)
{
    if(process_is_zero(process) || WaitForSingleObject((HANDLE)process.id[0], 0) != WAIT_OBJECT_0)
    {
        return 0;
    }
    DWORD exit_code = 0;
    bool result = GetExitCodeProcess((HANDLE)process.id[0], &exit_code);
    if (result && exit_code_out != 0) { *exit_code_out = exit_code; }
    return result;
}

internal bool process_is_active(Process process)
{
    return !process_is_zero(process) && !process_poll(process, 0);
}

internal bool process_join(Process process, uint64_t endt_us, uint64_t *exit_code_out)
{
    HANDLE process_handle = (HANDLE)(process.id[0]);
    DWORD sleep_ms = _win32_sleep_ms_from_endt_us(endt_us);
    DWORD result = WaitForSingleObject(process_handle, sleep_ms);
    bool process_joined = (result == WAIT_OBJECT_0);
    if(process_joined && exit_code_out)
    {
        DWORD exit_code = 0;
        if(GetExitCodeProcess(process_handle, &exit_code))
        {
            *exit_code_out = exit_code;
        }
    }
    if(process_joined)
    {
        CloseHandle(process_handle);
    }
    return process_joined;
}

internal void process_detach(Process process)
{
    HANDLE process_handle = (HANDLE)(process.id[0]);
    CloseHandle(process_handle);
}

internal bool process_kill(Process process)
{
    HANDLE process_handle = (HANDLE)process.id[0];
    BOOL was_terminated = TerminateProcess(process_handle, 999);
    return was_terminated;
}

internal bool process_send_ctrl_c(Process process)
{
    bool result = 0;
    if(process_is_zero(process)) { return 0; }
    bool had_console = GetConsoleCP() != 0;
    if(had_console && !FreeConsole()) { return 0; }
    if(AttachConsole(safe_cast_u32(pid_from_process(process)))) {
        SetConsoleCtrlHandler(0, TRUE);
        result = GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0) != 0;
        if(result) { Sleep(10); }
    }
    FreeConsole();
    if(had_console) { AttachConsole(ATTACH_PARENT_PROCESS); }
    SetConsoleCtrlHandler(0, FALSE);
    return result;
}

/// ak: Processes Group

internal Process_Group process_group_make(bool kill_on_close)
{
    Process_Group result = STRUCT_ZERO;
    HANDLE handle = CreateJobObjectW(0, 0);
    if(handle != 0)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits = STRUCT_ZERO;
        if(kill_on_close) { limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE; }
        if(SetInformationJobObject(handle, JobObjectExtendedLimitInformation, &limits, sizeof(limits)))
        {
            result.id[0] = (uint64_t)handle;
        }
        else
        {
            CloseHandle(handle);
        }
    }
    return result;
}

internal bool process_group_add(Process_Group group, Process process)
{
    bool result = group.id[0] != 0 && !process_is_zero(process) &&
        AssignProcessToJobObject((HANDLE)group.id[0], (HANDLE)process.id[0]);
    return result;
}

internal void process_group_close(Process_Group group)
{
    if(group.id[0] != 0) { CloseHandle((HANDLE)group.id[0]); }
}

//~ ak: OS Entry Points
//=============================================================================

int main(void)
{
    // ak: try to allow large pages if we can
    bool large_pages_allowed = 0;
    {
        HANDLE token;
        if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        {
            LUID luid;
            if(LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &luid))
            {
                TOKEN_PRIVILEGES priv;
                priv.PrivilegeCount           = 1;
                priv.Privileges[0].Luid       = luid;
                priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                large_pages_allowed = !!AdjustTokenPrivileges(token, 0, &priv, sizeof(priv), 0, 0);
            }
            CloseHandle(token);
        }
    }
    
    // ak: get system info
    SYSTEM_INFO sysinfo = STRUCT_ZERO;
    GetSystemInfo(&sysinfo);
    
    // ak: set up non-dynamically-alloc'd state
    //
    // (we need to set up some basics before this layer can supply
    // memory allocation primitives)
    {
        _win32_state.microsecond_resolution  = 1;
        LARGE_INTEGER large_int_resolution;
        if(QueryPerformanceFrequency(&large_int_resolution))
        {
            _win32_state.microsecond_resolution = large_int_resolution.QuadPart;
        }
    }
    {

        MEMORYSTATUSEX memory = { .dwLength = sizeof(memory) };
        GlobalMemoryStatusEx(&memory);

        System_Info *info = &_os_state.system_info;
        info->logical_processor_count = (uint64_t)sysinfo.dwNumberOfProcessors;
        info->page_size               = sysinfo.dwPageSize;
        info->large_page_size         = GetLargePageMinimum();
        info->allocation_granularity  = sysinfo.dwAllocationGranularity;
        info->physical_memory_size    = memory.ullTotalPhys;
    }
    {
        Process_Info *info = &_os_state.process_info;
        info->large_pages_allowed = large_pages_allowed;
        info->pid = GetCurrentProcessId();
    }
    
    // ak: set up dynamically allocated state
    _win32_state.arena = arena_alloc();
    
    //- ak: Setup argument array
    {
        int args_count;
        LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &args_count);
        _os_state.args = array_alloc(_win32_state.arena, Str8_Array, args_count, 0);
        for (int i = 0; i < args_count; i++)
        {
            Str16 str16 = str16_from_cstr(args[i]);
            Str8 str = str8_from_16(_win32_state.arena, str16);
            array_append(&_os_state.args, str);
        }
    }
    
    // NOTE(ak): we need this to set now time.
    {
        _win32_state.microsecond_resolution  = 1;
        LARGE_INTEGER large_int_resolution;
        if (QueryPerformanceFrequency(&large_int_resolution))
        {
            _win32_state.microsecond_resolution = large_int_resolution.QuadPart;
        }
    }
    
    {
        Process_Info *info = &_os_state.process_info;
        {
            Arena_Temp scratch = arena_scratch_begin(NULL, 0);
            DWORD size = KB(32);
            uint16_t *buffer = arena_push_nz(scratch.arena, uint16_t, size);
            DWORD length = GetModuleFileNameW(0, (WCHAR*)buffer, size);
            Str8 name8 = str8_from_16(scratch.arena, str16_init(buffer, length));
            info->binary_file_path = str8_copy(_win32_state.arena, name8);
            info->binary_path = str8_chop_last_slash(info->binary_file_path);
            arena_scratch_end(scratch);
        }
        info->initial_path = process_path_get(_win32_state.arena);
        {
            Arena_Temp scratch = arena_scratch_begin(NULL, 0);
            size_t size = KB(32);
            uint16_t *buffer = arena_push_nz(scratch.arena, uint16_t, size);
            if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_APPDATA, 0, 0, (WCHAR*)buffer)))
            {
                info->user_program_config_data_path = str8_from_16(_win32_state.arena, str16_from_cstr(buffer));
                info->user_program_cache_data_path = info->user_program_logs_data_path = info->user_program_config_data_path;
            }
            arena_scratch_end(scratch);
        }
        {
            WCHAR *this_proc_env = GetEnvironmentStringsW();
            size_t start_idx = 0;
            for(size_t idx = 0;; idx += 1)
            {
                if(this_proc_env[idx] == 0)
                {
                    if(start_idx == idx)
                    {
                        break;
                    }
                    else
                    {
                        Str16 string16 = str16_init((uint16_t *)this_proc_env + start_idx, idx - start_idx);
                        Str8 string = str8_from_16(_win32_state.arena, string16);
                        str8_list_push(_win32_state.arena, &info->environment, string);
                        start_idx = idx+1;
                    }
                }
            }
        }
    }
    
    // ak: initialize stdout/stdin/stderr
    FS_STDOUT = HandlePack(Fs_File, GetStdHandle(STD_OUTPUT_HANDLE));
    FS_STDIN = HandlePack(Fs_File, GetStdHandle(STD_INPUT_HANDLE));
    FS_STDERR = HandlePack(Fs_File, GetStdHandle(STD_ERROR_HANDLE));
    
    //- ak: Go to default OS entry point
    os_main();
}
