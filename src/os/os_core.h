#ifndef OS_CORE_H
#define OS_CORE_H

// External Includes
//=============================================================================

#include <fcntl.h>
#include <errno.h>

// Types
//=============================================================================

typedef uint64_t Os_File;

// File Property ==============================================================

typedef uint32_t FilePropertyFlags;
enum
{
    FilePropertyFlag_IsFolder = (1 << 0),
};

typedef struct Os_FileProperties Os_FileProperties;
struct Os_FileProperties
{
    uint64_t size;
    DenseTime modified;
    DenseTime created;
    FilePropertyFlags flags;
};

// Access Flags ===============================================================

typedef uint32_t Os_AccessFlags;
enum
{
    OS_AccessFlag_Read        = (1<<0),
    OS_AccessFlag_Write       = (1<<1),
    OS_AccessFlag_Execute     = (1<<2),
    OS_AccessFlag_Append      = (1<<3),
    OS_AccessFlag_ShareRead   = (1<<4),
    OS_AccessFlag_ShareWrite  = (1<<5),
    OS_AccessFlag_Inherited   = (1<<6),
};

// Private OS State ===========================================================

typedef struct _Os_Core_State _Os_Core_State;
struct _Os_Core_State {
    Str8Array args;
    Alloc alloc;
    Log_Context log_context;
};

// Functions
//=============================================================================

// Memory Allocation ==========================================================

internal void *os_memory_create(uint64_t size);
internal bool os_memory_commit(void *ptr, uint64_t size);
internal void os_memory_decommit(void *ptr, uint64_t size);
internal void *os_memory_alloc(uint64_t size);
internal void os_memory_free(void *ptr, uint64_t size);

// File System ================================================================

internal Os_File os_file_open(Str8 path, Os_AccessFlags flags);
internal void os_file_close(Os_File file);
internal uint64_t os_file_read(Os_File file, Rng1_U64 rng, void *out_data);
internal Str8 os_file_read_str(Os_File file, Rng1_U64 range, Alloc alloc);
internal Str8 os_file_read_str_full(Os_File file, Alloc alloc);
internal uint64_t os_file_write(Os_File file, Rng1_U64 rng, void *data);
internal Os_FileProperties os_file_properties(Os_File file);
internal bool os_dir_make(Str8 path);

// Exit =======================================================================

internal void os_exit(int32_t exit_code);

// Time =======================================================================

internal uint32_t os_now_unix(void);
internal void os_sleep_ms(uint32_t microsec);
internal void os_sleep_millisec(uint32_t millisec);

// Command line arguments =====================================================

internal Str8List *os_agrs_get(void);
internal Str8 *os_program_path_get(void);

// Environment Variable =======================================================

internal bool os_env_is_set(Str8 name);
internal Str8 os_env_get(Str8 name);

// Program Entry Points =======================================================

internal void os_entry_point(void);
internal void entry_point(void);

// Global Variables
//=============================================================================

global _Os_Core_State _os_core_state = ZERO_STRUCT;

#endif // OS_CORE_H
