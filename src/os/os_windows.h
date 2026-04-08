#ifndef OS_WINDOWS_H
#define OS_WINDOWS_H

//~ ak: External Includes
//=============================================================================

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>

//~ ak: Types
//=============================================================================

typedef struct _OS_Win32_State _OS_Win32_State;
struct _OS_Win32_State
{
    uint64_t microsecond_resolution;
};

typedef struct _Os_Win32_Walk_Iter _Os_Win32_Walk_Iter;
struct _Os_Win32_Walk_Iter
{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
    bool is_volume_iter;
    Str8_Array drive_strings;
    uint64_t drive_strings_iter_idx;
};

//~ ak: Defines
//=============================================================================

#define OS_STDIN  (Os_File)GetStdHandle(STD_INPUT_HANDLE)
#define OS_STDOUT (Os_File)GetStdHandle(STD_OUTPUT_HANDLE)
#define OS_STDERR (Os_File)GetStdHandle(STD_ERROR_HANDLE)

//~ ak: Functions
//=============================================================================

// Helpers functions ==========================================================

internal uint32_t _os_win32_unix_time_from_file_time(FILETIME file_time);
internal FilePropertyFlags _os_win32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);
internal void _os_win32_dense_time_from_file_time(DenseTime *out, FILETIME *in);
internal void _os_win32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in);

//~ ak: Global Variables
//=============================================================================

global _OS_Win32_State _os_win32_state = ZERO_STRUCT;

#endif // OS_WINDOWS_H
