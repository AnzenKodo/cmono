#ifndef OS_LINUX_H
#define OS_LINUX_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Memory Allocation
//====================================================================

fn void *os_memory_create(U64 size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) { result = 0; }
    return result;
}

fn I32 os_memory_commit(void *ptr, U64 size)
{
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

fn void os_memory_decommit(void *ptr, U64 size)
{
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

fn void *os_memory_alloc(U64 size)
{
    void *result = os_memory_create(size);
    os_memory_commit(result, size);
    return result;
}

fn void os_memory_free(void *ptr, U64 size)
{
    munmap(ptr, size);
}

// File System
//====================================================================

fn bool os_dir_make(Str8 path)
{
    I32 result = mkdir(path.str, 0700);

    if (result == 0) {
        return true;
    } else {
        return false;
    }
}

// Exit
//====================================================================

fn void os_exit(I32 exit_code)
{
    _exit(exit_code);
}

#endif // STD_OS_LINUX_H
