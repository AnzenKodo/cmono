//-----------------------------------------------------------------------------
// Reference:
// - By Epic Games Tools & Ryan Fleury: https://github.com/EpicGames/raddebugger/tree/e28a543573277315ffd32a3e983c8171570bb946/src/linux/base
// - By Allen Webster: https://github.com/4coder-archive/4coder/tree/master/platform_linux
//-----------------------------------------------------------------------------

// ak: External Includes
//=============================================================================

#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <time.h>
#include <spawn.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/sysinfo.h>

extern char **environ;

// ak: Private Types
//=============================================================================

//- ak: File System ===========================================================

//- ak: directory walking
typedef struct _Linux_Walk _Linux_Walk;
struct _Linux_Walk
{
    DIR *dir;
    struct dirent *dp;
    Str8 path;
};

//- ak: Processes =============================================================

typedef struct _Linux_Process_Group _Linux_Process_Group;
struct _Linux_Process_Group
{
    pid_t pid;
    bool kill_on_close;
};

//- ak: Linux State ===========================================================

typedef struct _Linux_State _Linux_State;
struct _Linux_State
{
    Arena *arena;
    char **default_env;
    size_t default_env_count;
};

// ak: Globals
//=============================================================================

global _Linux_State _linux_state = STRUCT_ZERO;

//~ ak: Macros
//=============================================================================

// ak: Linux Call Interruption Retry Helper
#define _LinuxRetryOnEintr(expr)             \
(__extension__({                           \
__typeof__(expr) __ret;                    \
do {                                       \
__ret = (expr);                          \
} while ((__ret == -1) && errno == EINTR); \
__ret;                                     \
}))

// ak: Private Function Definition
//=============================================================================

//- ak: Helper ================================================================

internal DateTime _linux_date_time_from_tm(struct tm in, uint32_t msec);
internal DenseTime _linux_dense_time_from_timespec(struct timespec in);
internal Fs_File_Properties _linux_properties_from_stat(struct stat *s);

// ak: Function Implementation
//=============================================================================

//- ak: Helper ================================================================

internal DateTime _linux_date_time_from_tm(struct tm in, uint32_t msec)
{
    DateTime dt = STRUCT_ZERO;
    dt.sec  = in.tm_sec;
    dt.min  = in.tm_min;
    dt.hour = in.tm_hour;
    dt.day  = in.tm_mday-1;
    dt.mon  = in.tm_mon;
    dt.year = in.tm_year+1900;
    dt.msec = msec;
    return dt;
}

internal DenseTime _linux_dense_time_from_timespec(struct timespec in)
{
    DenseTime result = 0;
    {
        struct tm tm_time = STRUCT_ZERO;
        gmtime_r(&in.tv_sec, &tm_time);
        DateTime date_time = _linux_date_time_from_tm(
            tm_time, in.tv_nsec/Million(1)
        );
        result = dense_time_from_date_time(date_time);
    }
    return result;
}

internal Fs_File_Properties _linux_properties_from_stat(struct stat *s)
{
    Fs_File_Properties props = STRUCT_ZERO;
    props.size     = s->st_size;
    props.created  = _linux_dense_time_from_timespec(s->st_ctim);
    props.modified = _linux_dense_time_from_timespec(s->st_mtim);
    if (s->st_mode & S_IFDIR)
    {
        props.flags |= Fs_File_Property_Flag_IsFolder;
    }
    return props;
}

// ak: Os Functions for Base Layer
//=============================================================================

//- ak: Memory ================================================================

internal void *mem_reserve(size_t size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (result == MAP_FAILED)
    { result = 0; }
    return result;
}
internal bool mem_commit(void *ptr, size_t size)
{
    int result = mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return result == 0;
}

internal bool mem_decommit(void *ptr, size_t size)
{
    int result1 = madvise(ptr, size, MADV_DONTNEED);
    int result2 = mprotect(ptr, size, PROT_NONE);
    return result1 == 0 && result2 == 0;
}
internal bool mem_release(void *ptr, size_t size)
{
    int result = munmap(ptr, size);
    return result == 0;
}

// ak: File System ============================================================

/// ak: File Operations

internal Fs_File fs_file_open(Str8 path, Fs_Access_Flags flags)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    int lnx_flags = 0;
    lnx_flags |= O_CLOEXEC;
    
    if(flags & Fs_Access_Flag_Read && flags & Fs_Access_Flag_Write)
    {
        lnx_flags = O_RDWR;
    }
    else if(flags & Fs_Access_Flag_Write)
    {
        lnx_flags = O_WRONLY;
    }
    else if(flags & Fs_Access_Flag_Read)
    {
        lnx_flags = O_RDONLY;
    }
    
    if(flags & Fs_Access_Flag_Append)
    {
        lnx_flags |= O_APPEND;
    }
    else if(flags & Fs_Access_Flag_Write)
    {
        lnx_flags |= O_TRUNC;
    }
    
    if(flags & (Fs_Access_Flag_Write|Fs_Access_Flag_Append))
    {
        lnx_flags |= O_CREAT;
    }
    
    if(flags & Fs_Access_Flag_CreateNew) { lnx_flags |= O_CREAT|O_EXCL; }
    
    Str8 path_copy = str8_copy(scratch.arena, path);
    int fd = open((char *)path_copy.cstr, lnx_flags, 0755);
    Fs_File handle = STRUCT_ZERO;
    if(fd != -1)
    {
        handle = HandlePack(Fs_File, fd);;
    }
    
    arena_scratch_end(scratch);
    return handle;
}

internal bool file_flush(Fs_File file)
{
  return fsync((int)file.id[0]) == 0;
}

internal void fs_file_close(Fs_File file)
{
  if(fs_file_is_zero(file)) { return; }
  int fd = (int)file.id[0];
  close(fd);
}

internal uint64_t fs_file_read(Fs_File file, Rng1_U64 rng, void *out_data)
{
    if(fs_file_is_zero(file)) { return 0; }
    int fd = (int)file.id[0];
    size_t total_num_bytes_to_read = dim_rng1(rng);
    size_t total_num_bytes_read = 0;
    size_t total_num_bytes_left_to_read = total_num_bytes_to_read;
    while (total_num_bytes_left_to_read > 0)
    {
        int read_result = pread(
            fd, (uint8_t *)out_data + total_num_bytes_read,
            total_num_bytes_left_to_read, rng.min + total_num_bytes_read
        );
        if (read_result >= 0)
        {
            total_num_bytes_read += read_result;
            total_num_bytes_left_to_read -= read_result;
        }
        else if (errno != EINTR)
        {
            break;
        }
    }
    return total_num_bytes_read;
}

internal size_t fs_file_write(Fs_File file, void *data, Rng1_U64 rng)
{
    if(fs_file_is_zero(file)) { return 0; }
    int fd = (int)file.id[0];
    size_t total_num_bytes_to_write = dim_rng1(rng);
    size_t total_num_bytes_written = 0;
    size_t total_num_bytes_left_to_write = total_num_bytes_to_write;
    while (total_num_bytes_left_to_write > 0)
    {
        int write_result = pwrite(
            fd, (uint8_t *)data + total_num_bytes_written,
            total_num_bytes_left_to_write, rng.min + total_num_bytes_written
        );
        if (write_result < 0 && errno == ESPIPE)
        {
            write_result = write(fd, (uint8_t *)data + total_num_bytes_written,
                total_num_bytes_left_to_write);
        }
        if (write_result >= 0)
        {
            total_num_bytes_written += write_result;
            total_num_bytes_left_to_write -= write_result;
        }
        else if (errno != EINTR)
        {
            break;
        }
    }
    return total_num_bytes_written;
}

internal Fs_File_Properties fs_file_properties_from_file(Fs_File file)
{
    if(fs_file_is_zero(file)) { return StructZeroType(Fs_File_Properties); }
    int fd = file.id[0];
    struct stat fd_stat = STRUCT_ZERO;
    int fstat_result = fstat(fd, &fd_stat);
    Fs_File_Properties props = STRUCT_ZERO;
    if (fstat_result != -1)
    {
        props = _linux_properties_from_stat(&fd_stat);
    }
    return props;
}

/// ak: Path Operations

internal bool fs_file_path_exists(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    Str8 path_copy = str8_copy(scratch.arena, path);
    int access_result = access((char *)path_copy.cstr, F_OK);
    bool result = 0;
    if(access_result == 0)
    {
        result = 1;
    }
    arena_scratch_end(scratch);
    return result;
}

internal bool fs_file_path_move(Str8 dst, Str8 src)
{
    bool good = 0;
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    {
        char *src_cstr = (char *)str8_copy(scratch.arena, src).cstr;
        char *dst_cstr = (char *)str8_copy(scratch.arena, dst).cstr;
        int rename_result = renameat(AT_FDCWD, src_cstr, AT_FDCWD, dst_cstr);
        good = (rename_result != -1);
    }
    arena_scratch_end(scratch);
    return good;
}

internal bool fs_file_path_copy(Str8 dst, Str8 src)
{
    bool result = 0;
    Fs_File src_h = fs_file_open(src, Fs_Access_Flag_Read);
    Fs_File dst_h = fs_file_open(dst, Fs_Access_Flag_Write);
    if(!fs_file_is_zero(src_h) && !fs_file_is_zero(dst_h))
    {
        int src_fd = (int)src_h.id[0];
        int dst_fd = (int)dst_h.id[0];
        Fs_File_Properties src_props = fs_file_properties_from_file(src_h);
        size_t size = src_props.size;
        size_t total_bytes_copied = 0;
        size_t bytes_left_to_copy = size;
        for(;bytes_left_to_copy > 0;)
        {
            off_t sendfile_off = total_bytes_copied;
            int send_result = sendfile(dst_fd, src_fd, &sendfile_off, bytes_left_to_copy);
            if(send_result <= 0)
            {
                break;
            }
            size_t bytes_copied = (size_t)send_result;
            bytes_left_to_copy -= bytes_copied;
            total_bytes_copied += bytes_copied;
        }
    }
    fs_file_close(src_h);
    fs_file_close(dst_h);
    return result;
}

internal bool fs_file_path_replace(Str8 dst, Str8 src)
{
    Arena_Temp scratch = arena_scratch_begin(0,0);
    char *dst_cstr = (char *)str8_copy(scratch.arena, dst).cstr;
    char *src_cstr = (char *)str8_copy(scratch.arena, src).cstr;
    bool   result   = rename(src_cstr, dst_cstr) == 0;
    arena_scratch_end(scratch);
    return result;
}

internal bool fs_file_path_delete(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    bool result = 0;
    Str8 path_copy = str8_copy(scratch.arena, path);
    if(remove((char *)path_copy.cstr) != -1)
    {
        result = 1;
    }
    arena_scratch_end(scratch);
    return result;
}

internal Str8 fs_full_path_from_path(Arena *arena, Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(&arena, 1);
    Str8 path_copy = str8_copy(scratch.arena, path);
    char buffer[PATH_MAX] = {0};
    realpath((char *)path_copy.cstr, buffer);
    Str8 result = str8_copy(arena, str8_from_cstr(buffer));
    arena_scratch_end(scratch);
    return result;
}

internal Fs_File_Properties fs_file_properties_from_file_path(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    Str8 path_copy = str8_copy(scratch.arena, path);
    struct stat f_stat = STRUCT_ZERO;
    int stat_result = stat((char *)path_copy.cstr, &f_stat);
    Fs_File_Properties props = STRUCT_ZERO;
    if(stat_result != -1)
    {
        props = _linux_properties_from_stat(&f_stat);
    }
    arena_scratch_end(scratch);
    return props;
}

/// ak: Directory Operations

internal bool fs_is_dir_exist(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    bool exists = 0;
    Str8  path_copy = str8_copy(scratch.arena, path);
    DIR *handle = opendir((char *)path_copy.cstr);
    if(handle)
    {
        closedir(handle);
        exists = 1;
    }
    arena_scratch_end(scratch);
    return exists;
}

internal bool fs_dir_make(Str8 path)
{
    Arena_Temp scratch = arena_scratch_begin(NULL, 0);
    bool result = 0;
    Str8 path_copy = str8_copy(scratch.arena, path);
    if(mkdir((char *)path_copy.cstr, 0755) != -1)
    {
        result = 1;
    }
    else
    {
        // match windows behavior
        result = fs_file_path_exists(path);
    }
    arena_scratch_end(scratch);
    return result;
}

/// ak: File System Walk

internal Fs_Walk *fs_walk_begin(Arena *arena, Str8 path, Fs_Walk_Flags flags)
{
    Fs_Walk *base_walk = arena_push(arena, Fs_Walk, 1);
    base_walk->flags = flags;
    _Linux_Walk *walk = (_Linux_Walk *)base_walk->memory;
    {
        Str8 path_copy = str8_copy(arena, path);
        walk->dir = opendir((char *)path_copy.cstr);
        walk->path = path_copy;
    }
    return base_walk;
}

internal bool fs_walk_next(Arena *arena, Fs_Walk *walk, Fs_Info *info_out)
{
    bool good = 0;
    _Linux_Walk *linux_walk = (_Linux_Walk *)walk->memory;
    while (true)
    {
        // ak: get next entry
        linux_walk->dp = readdir(linux_walk->dir);
        good = (linux_walk->dp != 0);
        // ak: unpack entry info
        struct stat st = STRUCT_ZERO;
        int stat_result = 0;
        if (good)
        {
            Arena_Temp scratch = arena_scratch_begin(&arena, 1);
            Str8 full_path = str8f(scratch.arena, "%.*s/%s", str8_varg(linux_walk->path), linux_walk->dp->d_name);
            stat_result = stat((char *)full_path.cstr, &st);
            arena_scratch_end(scratch);
        }
        // ak: determine if filtered
        bool filtered = 0;
        if (good)
        {
            filtered = ((st.st_mode == S_IFDIR && walk->flags & Fs_Walk_Flag_SkipFolders) ||
                    (st.st_mode == S_IFREG && walk->flags & Fs_Walk_Flag_SkipFiles) ||
                    (linux_walk->dp->d_name[0] == '.' && linux_walk->dp->d_name[1] == 0) ||
                    (linux_walk->dp->d_name[0] == '.' && linux_walk->dp->d_name[1] == '.' && linux_walk->dp->d_name[2] == 0));
        }
        // ak: output & exit, if good & unfiltered
        if (good && !filtered)
        {
            info_out->name = str8_copy(arena, str8_from_cstr(linux_walk->dp->d_name));
            if (stat_result != -1)
            {
                info_out->props = _linux_properties_from_stat(&st);
            }
            break;
        }
        // ak: exit if not good
        if (!good)
        {
            break;
        }
    }
    return good;
}

internal void fs_walk_end(Fs_Walk *walk)
{
    _Linux_Walk *linux_walk = (_Linux_Walk *)walk->memory;
    closedir(linux_walk->dir);
}

//- ak: Time ===================================================================

internal uint32_t time_now_unix(void)
{
    time_t t = time(0);
    return (uint32_t)t;
}

internal uint64_t time_now_us(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
    return result;
}

//~ ak: Processes =============================================================

/// ak: Process Management

internal void process_sleep_us(uint64_t microsec)
{
    struct timespec ts;
    ts.tv_sec = (time_t)(microsec / Million(1));
    ts.tv_nsec = (long)((microsec % Million(1)) * Thousand(1));
    nanosleep(&ts, NULL);
}

internal void process_sleep_ms(uint32_t millisec)
{
    usleep(millisec*Thousand(1));
}

internal void process_exit(int32_t exit_code)
{
    exit(exit_code);
}

/// ak: Process Info

internal Str8 process_path_get(Arena *arena)
{
    char *cwdir = getcwd(0, 0);
    Str8 string = str8_copy(arena, str8_from_cstr(cwdir));
    free(cwdir);
    return string;
}

internal uint32_t process_start_time_unix_get(void)
{
    Arena_Temp scratch = arena_scratch_begin(NULL,0);
    uint64_t start_time = 0;
    pid_t pid = getpid();
    Str8 path = str8f(scratch.arena, "/proc/%u", pid);
    struct stat st;
    int err = stat((char *)path.cstr, &st);
    if(err == 0)
    {
        start_time = st.st_mtime;
    }
    arena_scratch_end(scratch);
    return (uint32_t)start_time;
}

/// ak: Child Processes

internal Process process_init(Process_Launch_Params *params)
{
    Process process = STRUCT_ZERO;
    posix_spawn_file_actions_t file_actions = STRUCT_ZERO;
    int file_actions_init_code = posix_spawn_file_actions_init(&file_actions);
    if(file_actions_init_code == 0)
    {
        Arena_Temp scratch = arena_scratch_begin(0, 0);
        if(params->path.size != 0)
        {
            int chdir_code = posix_spawn_file_actions_addchdir_np(&file_actions, (char *)cstr_push(params->path, scratch.arena).cstr);
            Assert(chdir_code == 0);
        }
        if(!fs_file_is_zero(params->stdout_file))
        {
            int stdout_code = posix_spawn_file_actions_adddup2(&file_actions, (int)params->stdout_file.id[0], STDOUT_FILENO);
            Assert(stdout_code == 0);
        }
        if(!fs_file_is_zero(params->stderr_file))
        {
            int stderr_code = posix_spawn_file_actions_adddup2(&file_actions, (int)params->stderr_file.id[0], STDERR_FILENO);
            Assert(stderr_code == 0);
        }
        if(!fs_file_is_zero(params->stdin_file))
        {
            int stdin_code = posix_spawn_file_actions_adddup2(&file_actions, (int)params->stdin_file.id[0], STDIN_FILENO);
            Assert(stdin_code == 0);
        }
        Fs_File std_files[] = { params->stdout_file, params->stderr_file, params->stdin_file };
        for EachIndex(i, ArrayLength(std_files))
        {
            int fd = (int)std_files[i].id[0];
            bool is_unique = !fs_file_is_zero(std_files[i]);
            for EachIndex(j, i) { is_unique &= !fs_file_match(std_files[i], std_files[j]); }
            if(is_unique && fd > STDERR_FILENO)
            {
                int close_code = posix_spawn_file_actions_addclose(&file_actions, fd);
                Assert(close_code == 0);
            }
        }
        posix_spawnattr_t attr = {0};
        int attr_init_code = posix_spawnattr_init(&attr);
        if(attr_init_code == 0)
        {
            if(params->new_console || params->process_group.id[0] != 0)
            {
                posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETPGROUP);
                posix_spawnattr_setpgroup(&attr, 0);
            }

            // package argv
            char **argv = arena_push(scratch.arena, char *, params->cmd_line.length + 1);
            {
                argv[0] = (char *)cstr_push(params->cmd_line.first->str, scratch.arena).cstr;
                size_t arg_idx = 1;
                for EachNode(n, Str8_Node, params->cmd_line.first->next)
                {
                    argv[arg_idx] = (char *)cstr_push(n->str, scratch.arena).cstr;
                    arg_idx += 1;
                }
            }

            // package envp
            char **envp = 0;
            if(params->inherit_env)
            {
                envp = _linux_state.default_env;
            }
            else
            {
                envp = arena_push(scratch.arena, char *, params->env.length + 2);
                size_t env_idx = 0;
                for EachNode(n, Str8_Node, params->cmd_line.first)
                {
                    envp[env_idx] = (char *)n->str.cstr;
                    env_idx += 1;
                }
            }

            // spawn process
            pid_t pid = 0;
            int spawn_code = posix_spawnp(&pid, argv[0], &file_actions, &attr, argv, envp);
            if(spawn_code == 0)
            {
                process.id[0] = (uint64_t)pid;
                if(params->process_group.id[0] != 0 && !process_group_add(params->process_group, process))
                {
                    kill(pid, SIGKILL);
                    waitpid(pid, 0, 0);
                    MemSetZeroStruct(&process);
                }
            }

            // clean up attributes
            int attr_destroy_code = posix_spawnattr_destroy(&attr);
        }
        arena_scratch_end(scratch);

        // clean up file actions
        int file_actions_destroy_code = posix_spawn_file_actions_destroy(&file_actions);
    }
    return process;
}

internal uint64_t pid_from_process(Process process)
{
    uint32_t result = process.id[0];
    return result;
}

internal bool process_poll(Process process, uint64_t *exit_code_out)
{
    if(process_match(process, StructZeroType(Process))) { return 0; }
    siginfo_t info = STRUCT_ZERO;
    bool result = waitid(P_PID, (pid_t)process.id[0], &info, WEXITED|WNOHANG|WNOWAIT) == 0 && info.si_pid != 0;
    if(result && exit_code_out != 0)
    {
        *exit_code_out = info.si_code == CLD_EXITED ? (uint64_t)info.si_status : (uint64_t)info.si_status + 128;
    }
    return result;
}

internal bool process_is_active(Process process)
{
    bool result = !process_match(process, StructZeroType(Process)) &&
        !process_poll(process, 0) && kill((pid_t)process.id[0], 0) == 0;
    return result;
}

internal bool process_join(Process process, uint64_t endt_us, uint64_t *exit_code_out)
{
    bool result = 0;

    pid_t pid = (pid_t)process.id[0];
    for(;;)
    {
        int status = 0;
        pid_t wait_result = _LinuxRetryOnEintr(waitpid(pid, &status, (endt_us == max_u64) ? 0 : WNOHANG));

        if((wait_result == pid) && (WIFEXITED(status) || WIFSIGNALED(status)))
        {
            result = 1;
            if(exit_code_out != 0)
            {
                if     (WIFEXITED(status))   { *exit_code_out = WEXITSTATUS(status); }
                else if(WIFSIGNALED(status)) { *exit_code_out = WTERMSIG(status) + 128; }
            }
            break;
        }

        if(wait_result == -1) { break; }
        if(endt_us == 0)      { break; }

        uint64_t now_us = time_now_us();
        if(now_us >= endt_us) { break; }

        uint64_t left_us  = endt_us - now_us;
        uint64_t sleep_us = Min(left_us, Thousand(1));
        usleep((useconds_t)sleep_us);
    }
    return result;
}

internal void process_detach(Process process)
{
    Unused(process);
    // no need to close pid
}

internal bool process_kill(Process process)
{
    int error_code = kill((pid_t)process.id[0], SIGKILL);
    bool is_killed = error_code == 0;
    return is_killed;
}

internal bool process_send_ctrl_c(Process process)
{
    bool result = !process_match(process, StructZeroType(Process)) && kill((pid_t)process.id[0], SIGINT) == 0;
    return result;
}

/// ak: Processes Group

internal Process_Group process_group_make(bool kill_on_close)
{
    _Linux_Process_Group *group = malloc(sizeof(*group));
    if(group != 0) { *group = (_Linux_Process_Group){ .kill_on_close = kill_on_close }; }
    return (Process_Group){ .id[0] = (uint64_t)group };
}

internal bool process_group_add(Process_Group group, Process process)
{
    _Linux_Process_Group *lnx_group = (_Linux_Process_Group *)group.id[0];
    if(lnx_group == 0 || lnx_group->pid != 0 || process_match(process, StructZeroType(Process))) { return 0; }
    lnx_group->pid = (pid_t)process.id[0];
    return 1;
}

internal void process_group_close(Process_Group group)
{
    _Linux_Process_Group *lnx_group = (_Linux_Process_Group *)group.id[0];
    if(lnx_group != 0)
    {
        if(lnx_group->kill_on_close && lnx_group->pid != 0)
        {
            kill(-lnx_group->pid, SIGKILL);
        }
        free(lnx_group);
    }
}

//~ ak: OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    // ak: get statically-allocated system/process info
    {
        uint64_t pages     = (uint64_t)sysconf(_SC_PHYS_PAGES);
        size_t page_size   = (size_t)sysconf(_SC_PAGESIZE);
        size_t commit_size = (size_t)sysconf(_SC_AVPHYS_PAGES);

        System_Info *info = &_os_state.system_info;
        info->logical_processor_count = (size_t)get_nprocs();
        info->page_size               = (size_t)getpagesize();
        info->large_page_size         = MB(2);
        info->allocation_granularity  = info->page_size;
        info->physical_memory_size    = pages * page_size;
    }
    {
        Process_Info *info = &_os_state.process_info;
        info->pid = (uint32_t)getpid();
    }
    
    // ak: set up dynamically allocated state
    _linux_state.arena = arena_alloc();
    
    // ak: make array of args
    {
        _os_state.args = array_alloc(_linux_state.arena, Str8_Array, (size_t)argc, 0);
        for (int i = 0; i < argc; i++)
        {
            Str8 str = str8_from_cstr(argv[i]);
            array_append(&_os_state.args, str);
        }
    }
    
    // ak: get system/process info
    Process_Info *info = &_os_state.process_info;
    info->pid = (uint32_t)getpid();
    
    // ak: cache default environment
    {
        size_t env_count = 0;
        for(; __environ[env_count] != 0; env_count += 1) {}

        char **default_env = arena_push(_linux_state.arena, char *, env_count+1);
        for EachIndex(idx, env_count)
        {
            Str8 entry = str8_copy(_linux_state.arena, str8_from_cstr(__environ[idx]));
            default_env[idx] = (char *)entry.cstr;
            str8_list_push(_linux_state.arena, &_os_state.process_info.environment, entry);
        }
        default_env[env_count] = 0;

        _linux_state.default_env_count = env_count;
        _linux_state.default_env       = default_env;
    }
    
    // ak: initialize stdout/stdin/stderr
    FS_STDIN = HandlePack(Fs_File, 0);
    FS_STDOUT = HandlePack(Fs_File, 1);
    FS_STDERR = HandlePack(Fs_File, 2);
     
    // ak: os entry point
    return os_main();
}
