#ifndef OS_CORE_H
#define OS_CORE_H

// External Includes
//=============================================================================

#include <fcntl.h>
#include <errno.h>

// Types
//=============================================================================

typedef uint64_t Os_File;

// Access Flags ===============================================================

typedef uint32_t Os_AccessFlags;
enum
{
    Os_AccessFlag_Read        = (1<<0),
    Os_AccessFlag_Write       = (1<<1),
    Os_AccessFlag_Execute     = (1<<2),
    Os_AccessFlag_Append      = (1<<3),
    Os_AccessFlag_ShareRead   = (1<<4),
    Os_AccessFlag_ShareWrite  = (1<<5),
    Os_AccessFlag_Inherited   = (1<<6),
};

//~ ak: File System ===========================================================

//- ak: File Property
typedef uint32_t Os_File_Property_Flags;
enum
{
    Os_File_Property_Flag_IsFolder = (1 << 0),
};

typedef struct Os_File_Properties Os_File_Properties;
struct Os_File_Properties
{
    uint64_t size;
    DenseTime modified;
    DenseTime created;
    Os_File_Property_Flags flags;
};

typedef struct Os_File_Info Os_File_Info;
struct Os_File_Info
{
    Str8 name;
    Os_File_Properties props;
};

//- ak: directory walking

typedef uint32_t Os_File_Walk_Flags;
enum
{
    Os_File_Walk_Flag_SkipFolders     = (1 << 0),
    Os_File_Walk_Flag_SkipFiles       = (1 << 1),
    Os_File_Walk_Flag_SkipHiddenFiles = (1 << 2),
    Os_File_Walk_Flag_Done            = (1 << 31),
};

typedef struct Os_File_Walk Os_File_Walk;
struct Os_File_Walk
{
    Os_File_Walk_Flags flags;
    uint8_t memory[800];
};

// Private OS State ===========================================================

typedef struct _Os_Core_State _Os_Core_State;
struct _Os_Core_State {
    Str8_Array args;
    Log_Context log_context;
};

// Functions
//=============================================================================

// Memory Allocation ==========================================================

internal void *os_memory_alloc(size_t size);
internal void *os_memory_reserve(size_t size);
internal bool os_memory_release(void *ptr, size_t size);
internal bool os_memory_commit(void *ptr, size_t size);
internal bool os_memory_decommit(void *ptr, size_t size);
internal size_t os_pagesize_get(void);

//~ ak: File System ===========================================================

//- ak: Open and Close file
internal Os_File os_file_open(Str8 path, Os_AccessFlags flags);
internal void os_file_close(Os_File file);

//- ak: File Read
internal size_t os_file_read(Os_File file, Rng1_U64 rng, void *out_data);
internal size_t os_file_read_full(Os_File file, void *out_data);
internal Str8 os_file_read_str(Os_File file, Rng1_U64 range, Arena *arena);
internal Str8 os_file_read_str_full(Os_File file, Arena *arena);
internal Str8 os_path_read_str(Str8 path, Rng1_U64 range, Arena *arena);
internal Str8 os_path_read_str_full(Str8 path, Arena *arena);

//- ak: File Write
internal size_t os_file_write(Os_File file, void *data, Rng1_U64 rng);
internal size_t os_file_write_append(Os_File file, void *data, size_t size);
internal Os_File_Properties os_file_properties(Os_File file);
internal bool os_dir_make(Str8 path);

//- ak: directory walking
internal Os_File_Walk *os_file_walk_begin(Arena *arena, Str8 path, Os_File_Walk_Flags flags);
internal bool os_file_walk_next(Arena *arena, Os_File_Walk *walk, Os_File_Info *info_out);
internal void os_file_walk_end(Os_File_Walk *walk);

// Exit =======================================================================

internal void os_exit(int32_t exit_code);

// Time =======================================================================

internal uint32_t os_now_unix(void);
internal void os_sleep_ms(uint32_t microsec);
internal void os_sleep_millisec(uint32_t millisec);

// Command line arguments =====================================================

internal Str8_List *os_agrs_get(void);
internal Str8 *os_program_path_get(void);

// Environment Variable =======================================================

internal bool os_env_is_set(Str8 name);
internal Str8 os_env_get(Str8 name);

// Program Entry Points =======================================================

internal void os_main(void);

// Global Variables
//=============================================================================

global _Os_Core_State _os_core_state = ZERO_STRUCT;

#endif // OS_CORE_H
