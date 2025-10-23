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

typedef struct OS_W32_State OS_W32_State;
struct OS_W32_State
{
    U64 microsecond_resolution;
};

// Functions
//=============================================================================

// Helpers Functions ==========================================================

internal U32 os_w32_unix_time_from_file_time(FILETIME file_time);
internal FilePropertyFlags os_w32_file_property_flags_from_dwFileAttributes(DWORD dwFileAttributes);
internal void os_w32_dense_time_from_file_time(DenseTime *out, FILETIME *in);
internal void os_w32_date_time_from_system_time(DateTime *out, SYSTEMTIME *in);

// Globals
//=============================================================================

global OS_W32_State os_w32_state = ZERO_STRUCT;

#endif // OS_WINDOWS_H
