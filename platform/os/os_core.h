#ifndef OS_CORE_H
#define OS_CORE_H

// Memory Allocation
//====================================================================

fn void *os_memory_create(U64 size);
fn I32 os_memory_commit(void *ptr, U64 size);
fn void os_memory_decommit(void *ptr, U64 size);
fn void *os_memory_alloc(U64 size);
fn void os_memory_free(void *ptr, U64 size);

// File System
//====================================================================

fn bool os_dir_make(Str8 path);

// Exit
//====================================================================

fn void os_exit(I32 exit_code);

#endif // OS_CORE_H
