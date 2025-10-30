#ifndef OS_WINDOWS_H
#define OS_WINDOWS_H

// External Includes
//=============================================================================

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>

// Types
//=============================================================================

typedef struct OS_Win32_State OS_Win32_State;
struct OS_Win32_State
{
    U64 microsecond_resolution;
};

// Defines
//=============================================================================

#define OS_STDIN  (Os_File)GetStdHandle(STD_INPUT_HANDLE)
#define OS_STDOUT (Os_File)GetStdHandle(STD_OUTPUT_HANDLE)
#define OS_STDERR (Os_File)GetStdHandle(STD_ERROR_HANDLE)

// Functions
//=============================================================================

// Helpers Functions ==========================================================

internal U32 os_w32_unix_time_from_file_time(FILETIME file_time);
internal FilePropertyFlags os_w32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);
internal void os_w32_dense_time_from_file_time(DenseTime *out, FILETIME *in);
internal void os_w32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in);

// Globals
//=============================================================================

global OS_Win32_State os_win32_state = ZERO_STRUCT;

#endif // OS_WINDOWS_H
