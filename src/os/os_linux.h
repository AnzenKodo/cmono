#ifndef OS_LINUX_H
#define OS_LINUX_H

// External Includes
//=============================================================================

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
extern char **environ;

// Defines
//=============================================================================

#define OS_STDIN  STDIN_FILENO
#define OS_STDOUT STDOUT_FILENO
#define OS_STDERR STDERR_FILENO

// Functions
//=============================================================================

// Helpers functions ==========================================================

internal DateTime _os_linux_date_time_from_tm(struct tm in, uint32_t msec);
internal DenseTime _os_linux_dense_time_from_timespec(struct timespec in);
internal Os_FileProperties _os_linux_file_properties_from_stat(struct stat *s);

#endif // OS_LINUX_H
