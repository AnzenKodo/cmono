// Helpers functions
//=============================================================================

internal DateTime _os_linux_date_time_from_tm(struct tm in, uint32_t msec)
{
    DateTime dt = ZERO_STRUCT;
    dt.sec  = in.tm_sec;
    dt.min  = in.tm_min;
    dt.hour = in.tm_hour;
    dt.day  = in.tm_mday-1;
    dt.mon  = in.tm_mon;
    dt.year = in.tm_year+1900;
    dt.msec = msec;
    return dt;
}

internal DenseTime _os_linux_dense_time_from_timespec(struct timespec in)
{
    DenseTime result = 0;
    {
        struct tm tm_time = ZERO_STRUCT;
        gmtime_r(&in.tv_sec, &tm_time);
        DateTime date_time = _os_linux_date_time_from_tm(
            tm_time, in.tv_nsec/Million(1)
        );
        result = dense_time_from_date_time(date_time);
    }
    return result;
}

internal Os_FileProperties _os_linux_file_properties_from_stat(struct stat *s)
{
    Os_FileProperties props = ZERO_STRUCT;
    props.size     = s->st_size;
    props.created  = _os_linux_dense_time_from_timespec(s->st_ctim);
    props.modified = _os_linux_dense_time_from_timespec(s->st_mtim);
    if(s->st_mode & S_IFDIR)
    {
        props.flags |= FilePropertyFlag_IsFolder;
    }
    return props;
}


// Memory Allocation
//=============================================================================

internal void *os_memory_create(uint64_t size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) { result = 0; }
    return result;
}

internal bool os_memory_commit(void *ptr, uint64_t size)
{
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return true;
}

internal void os_memory_decommit(void *ptr, uint64_t size)
{
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

internal void os_memory_free(void *ptr, uint64_t size)
{
    munmap(ptr, size);
}

// File System
//=============================================================================

internal Os_File os_file_open(Str8 path, Os_AccessFlags flags)
{
    int32_t access_flags = 0;
    if(flags & OS_AccessFlag_Read && flags & OS_AccessFlag_Write)
    {
        access_flags = O_RDWR;
    }
    else if(flags & OS_AccessFlag_Write)
    {
        access_flags = O_WRONLY|O_TRUNC;
    }
    else if(flags & OS_AccessFlag_Read)
    {
        access_flags = O_RDONLY;
    }
    if(flags & OS_AccessFlag_Append)
    {
        access_flags |= O_APPEND;
    }
    if(flags & (OS_AccessFlag_Write|OS_AccessFlag_Append))
    {
        access_flags |= O_CREAT;
    }
    Os_File file = open((char *)path.cstr, access_flags, 0666);
    if (!(flags & OS_AccessFlag_Inherited)) {
        fcntl(file, F_SETFD, FD_CLOEXEC);
    }
    // Lock file based on given flags
    short share_mode = 0;
    if (!(flags & OS_AccessFlag_ShareRead)) {
        share_mode |= F_RDLCK;
    }
    if (!(flags & OS_AccessFlag_ShareWrite)) {
        share_mode |= F_WRLCK;
    }
    if (share_mode) {
        struct flock lock = {0};
        lock.l_type = share_mode;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;  // Lock entire file
        fcntl(file, F_SETLK, &lock);
    }
    return file;
}

internal void os_file_close(Os_File file)
{
    close(file);
}

internal uint64_t os_file_read(Os_File file, Rng1_U64 rng, void *out_data)
{
    uint64_t total_num_bytes_to_read = dim1_u64(rng);
    uint64_t total_num_bytes_read = 0;
    uint64_t total_num_bytes_left_to_read = total_num_bytes_to_read;
    for(;total_num_bytes_left_to_read > 0;)
    {
        int read_result = pread(
            file, (uint8_t *)out_data + total_num_bytes_read,
            total_num_bytes_left_to_read, rng.min + total_num_bytes_read
        );
        if(read_result >= 0)
        {
            total_num_bytes_read += read_result;
            total_num_bytes_left_to_read -= read_result;
        }
        else if(errno != EINTR)
        {
            break;
        }
    }
    return total_num_bytes_read;
}

internal uint64_t os_file_write(Os_File file, Rng1_U64 rng, void *data)
{
    uint64_t total_num_bytes_to_write = dim1_u64(rng);
    uint64_t total_num_bytes_written = 0;
    if (file == OS_STDOUT || file == OS_STDIN || file == OS_STDERR)
    {
        total_num_bytes_written = write(file, data, total_num_bytes_to_write);
    }
    else
    {
        uint64_t total_num_bytes_left_to_write = total_num_bytes_to_write;
        for(;total_num_bytes_left_to_write > 0;)
        {
            int write_result = pwrite(
                file, (uint8_t *)data + total_num_bytes_written,
                total_num_bytes_left_to_write, rng.min + total_num_bytes_written
            );
            if(write_result >= 0)
            {
                total_num_bytes_written += write_result;
                total_num_bytes_left_to_write -= write_result;
            }
            else if(errno != EINTR)
            {
                break;
            }
        }
    }
    return total_num_bytes_written;
}

internal Os_FileProperties os_file_properties(Os_File file)
{
    struct stat fd_stat = ZERO_STRUCT;
    int fstat_result = fstat(file, &fd_stat);
    Os_FileProperties props = ZERO_STRUCT;
    if(fstat_result != -1)
    {
        props = _os_linux_file_properties_from_stat(&fd_stat);
    }
    return props;
}

internal bool os_dir_make(Str8 path)
{
    int32_t result = mkdir((const char *)path.cstr, 0700);
    if (result == 0) {
        return true;
    } else {
        return false;
    }
}

// Exit
//=============================================================================

internal void os_exit(int32_t exit_code)
{
    exit(exit_code);
}

// Time
//=============================================================================

internal uint32_t os_now_unix(void)
{
    time_t t = time(0);
    return (uint32_t)t;
}

internal uint64_t os_now_microsec(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
    return result;
}

internal void os_sleep_microsec(uint64_t micosec)
{
    struct timespec ts = {
        .tv_sec = micosec / Million(1),
        .tv_nsec = (micosec % Million(1)) * Thousand(1),
    };
    nanosleep(&ts, NULL);
}

internal void os_sleep_millisec(uint32_t millisec)
{
    usleep(millisec*Thousand(1));
}

// Environment Variable =======================================================

internal bool os_env_is_set(Str8 name)
{
    bool result = false;
    for (char **e = environ; *e != NULL; e++) {
        Str8 env = str8_from_cstr(*e);
        uint64_t equal_pos = str8_find_substr(env, 0, str8("="), 0);
        Str8 env_name = str8_prefix(env, equal_pos);
        if (str8_match(env_name, name, 0)) {
            result = true;
        }
    }
    return result;
}

internal Str8 os_env_get(Str8 name)
{
    Str8 result = ZERO_STRUCT;
    for (char **e = environ; *e != NULL; e++) {
        Str8 env = str8_from_cstr(*e);
        uint64_t equal_pos = str8_find_substr(env, 0, str8("="), 0);
        if (os_env_is_set(name)) {
            result = str8_skip(env, equal_pos+1);
        }
    }
    return result;
}

// OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    uint64_t size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    _os_core_state.args = array_alloc(alloc, Str8Array, argc);
    _os_core_state.alloc = alloc;
    for(int i = 0; i < argc; i++)
    {
        Str8 str = str8_from_cstr(argv[i]);
        array_append(&_os_core_state.args, str);
    }
    os_entry_point();
}
