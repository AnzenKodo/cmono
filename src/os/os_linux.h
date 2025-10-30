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

// Defines
//=============================================================================

#define OS_STDIN  STDIN_FILENO
#define OS_STDOUT STDOUT_FILENO
#define OS_STDERR STDERR_FILENO

#endif // OS_LINUX_H
