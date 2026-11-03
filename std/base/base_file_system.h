//-----------------------------------------------------------------------------
// Reference:
// - By Epic Games Tools & Ryan Fleury
//      - https://github.com/EpicGames/raddebugger/blob/e28a543573277315ffd32a3e983c8171570bb946/src/base/base_files.h
//      - https://github.com/EpicGames/raddebugger/blob/e28a543573277315ffd32a3e983c8171570bb946/src/base/base_files.c
//-----------------------------------------------------------------------------

#ifndef BASE_FILE_H
#define BASE_FILE_H

// ak: Types
//=============================================================================

typedef uint32_t Fs_Access_Flags;
enum
{
    Fs_Access_Flag_Read        = (1<<0),
    Fs_Access_Flag_Write       = (1<<1),
    Fs_Access_Flag_Execute     = (1<<2),
    Fs_Access_Flag_Append      = (1<<3),
    Fs_Access_Flag_ShareRead   = (1<<4),
    Fs_Access_Flag_ShareWrite  = (1<<5),
    Fs_Access_Flag_Inherited   = (1<<6),
    Fs_Access_Flag_CreateNew   = (1<<7), // fail if file already exists; never truncates
};

typedef uint32_t Fs_Walk_Flags;
enum
{
    Fs_Walk_Flag_SkipFolders     = (1 << 0),
    Fs_Walk_Flag_SkipFiles       = (1 << 1),
    Fs_Walk_Flag_SkipHiddenFiles = (1 << 2),
    Fs_Walk_Flag_Done            = (1 << 31),
};

typedef struct Fs_Walk Fs_Walk;
struct Fs_Walk
{
    Fs_Walk_Flags flags;
    uint8_t memory[800];
};

typedef uint32_t Fs_File_Property_Flags;
enum
{
    Fs_File_Property_Flag_IsFolder = (1 << 0),
};

typedef struct Fs_File_Properties Fs_File_Properties;
struct Fs_File_Properties
{
    uint64_t size;
    DenseTime modified;
    DenseTime created;
    Fs_File_Property_Flags flags;
};

typedef struct Fs_Info Fs_Info;
struct Fs_Info
{
    Str8 name;
    Fs_File_Properties props;
};

// ak: on-disk file identifier
typedef struct Fs_File_Id Fs_File_Id;
struct Fs_File_Id
{
    uint64_t id[3];
};

typedef struct Fs_File Fs_File;
struct Fs_File
{
    uint64_t id[1];
};

typedef struct Fs_File_Pair Fs_File_Pair;
struct Fs_File_Pair
{
    Fs_File read;
    Fs_File write;
};

typedef struct Fs_File_Map Fs_File_Map;
struct Fs_File_Map
{
    uint64_t id[1];
};

// ak: Global Variables
//=============================================================================

global Fs_File FS_STDOUT;
global Fs_File FS_STDIN;
global Fs_File FS_STDERR;

// ak: Functions
//=============================================================================


//- ak: File System Helpers ===================================================

/// Handle Helpers
internal bool fs_file_match(Fs_File a, Fs_File b);
internal bool fs_file_is_zero(Fs_File file);

/// U8Array Helpers
internal U8Array fs_read_data_from_file_path(Str8 path, Arena *arena);
internal bool fs_write_data_to_file_path(Str8 path, U8Array data);
internal bool fs_write_data_list_to_file_path(Str8 path, Str8_List list);
internal bool fs_append_data_to_file_path(Str8 path, U8Array data);
internal U8Array fs_read_data_from_file_range(Arena *arena, Fs_File file, Rng1_U64 range);
internal Str8 file_read_cstring(Arena *arena, Fs_File file, size_t off);

/// Str8 Helpers
internal Str8 fs_read_str_from_file_path(Str8 path, Arena *arena);
internal bool fs_write_str_to_file_path(Str8 path, U8Array data);
internal bool fs_write_str_list_to_file_path(Str8 path, Str8_List list);

//- File System Core ==========================================================

/// ak: File Operations
internal Fs_File fs_file_open(Str8 path, Fs_Access_Flags flags);
internal bool file_flush(Fs_File file);
internal void fs_file_close(Fs_File file);
internal size_t fs_file_read(Fs_File file, Rng1_U64 rng, void *out_data);
#define file_read_struct(f, off, ptr) file_read((f), r1u64((off), (off)+sizeof(*(ptr))), (ptr))
internal size_t fs_file_write(Fs_File file, void *data, Rng1_U64 rng);
internal Fs_File_Properties fs_file_properties_from_file(Fs_File file);

/// ak: Path Operations
internal bool fs_file_path_exists(Str8 path);
internal bool fs_file_path_move(Str8 dst, Str8 src);
internal bool fs_file_path_copy(Str8 dst, Str8 src);
internal bool fs_file_path_replace(Str8 dst, Str8 src);
internal bool fs_file_path_delete(Str8 path);
internal Str8 fs_full_path_from_path(Arena *arena, Str8 path);
internal Fs_File_Properties fs_file_properties_from_file_path(Str8 path);

/// ak: Directory Operations
internal bool fs_dir_exists(Str8 path);
internal bool fs_dir_make(Str8 path);

/// ak: File System Walk
internal Fs_Walk *fs_walk_begin(Arena *arena, Str8 path, Fs_Walk_Flags flags);
internal bool fs_walk_next(Arena *arena, Fs_Walk *iter, Fs_Info *info_out);
internal void fs_walk_end(Fs_Walk *iter);

#endif // BASE_FILE_H
