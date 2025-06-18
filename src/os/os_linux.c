// Helpers Functions
//=============================================================================

internal DateTime
os_lnx_date_time_from_tm(struct tm in, U32 msec)
{
    DateTime dt = {0};
    dt.sec  = in.tm_sec;
    dt.min  = in.tm_min;
    dt.hour = in.tm_hour;
    dt.day  = in.tm_mday-1;
    dt.mon  = in.tm_mon;
    dt.year = in.tm_year+1900;
    dt.msec = msec;
    return dt;
}

internal DenseTime
os_lnx_dense_time_from_timespec(struct timespec in)
{
    DenseTime result = 0;
    {
        struct tm tm_time = {0};
        gmtime_r(&in.tv_sec, &tm_time);
        DateTime date_time = os_lnx_date_time_from_tm(
            tm_time, in.tv_nsec/Million(1)
        );
        result = dense_time_from_date_time(date_time);
    }
    return result;
}

internal Os_FileProperties
os_lnx_file_properties_from_stat(struct stat *s)
{
    Os_FileProperties props = {0};
    props.size     = s->st_size;
    props.created  = os_lnx_dense_time_from_timespec(s->st_ctim);
    props.modified = os_lnx_dense_time_from_timespec(s->st_mtim);
    if(s->st_mode & S_IFDIR)
    {
        props.flags |= FilePropertyFlag_IsFolder;
    }
    return props;
}


// Memory Allocation
//=============================================================================

internal void *
os_memory_create(U64 size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if(result == MAP_FAILED) { result = 0; }
    return result;
}

internal I32
os_memory_commit(void *ptr, U64 size)
{
    mprotect(ptr, size, PROT_READ|PROT_WRITE);
    return 1;
}

internal void
os_memory_decommit(void *ptr, U64 size)
{
    madvise(ptr, size, MADV_DONTNEED);
    mprotect(ptr, size, PROT_NONE);
}

internal void
os_memory_free(void *ptr, U64 size)
{
    munmap(ptr, size);
}

// File System
//=============================================================================

internal Os_File
os_file_open(Str8 path, Os_AccessFlags flags)
{
    I32 lnx_flags = 0;
    if(flags & OS_AccessFlag_Read && flags & OS_AccessFlag_Write)
    {
        lnx_flags = O_RDWR;
    }
    else if(flags & OS_AccessFlag_Write)
    {
        lnx_flags = O_WRONLY|O_TRUNC;
    }
    else if(flags & OS_AccessFlag_Read)
    {
        lnx_flags = O_RDONLY;
    }
    if(flags & OS_AccessFlag_Append)
    {
        lnx_flags |= O_APPEND;
    }
    if(flags & (OS_AccessFlag_Write|OS_AccessFlag_Append))
    {
        lnx_flags |= O_CREAT;
    }
    Os_File file = open((char *)path.str, lnx_flags, 0666);
    return file;
}

internal void
os_file_close(Os_File file)
{
    close(file);
}

internal U64
os_file_read(Os_File file, Rng1U64 rng, void *out_data)
{
    U64 total_num_bytes_to_read = dim_1u64(rng);
    U64 total_num_bytes_read = 0;
    U64 total_num_bytes_left_to_read = total_num_bytes_to_read;
    for(;total_num_bytes_left_to_read > 0;)
    {
        int read_result = pread(
            file, (U8 *)out_data + total_num_bytes_read,
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

internal U64
os_file_write(Os_File file, Rng1U64 rng, void *data)
{
    U64 total_num_bytes_to_write = dim_1u64(rng);
    U64 total_num_bytes_written = 0;
    U64 total_num_bytes_left_to_write = total_num_bytes_to_write;
    for(;total_num_bytes_left_to_write > 0;)
    {
        int write_result = pwrite(
            file, (U8 *)data + total_num_bytes_written,
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
    return total_num_bytes_written;
}

internal Os_FileProperties
os_file_properties(Os_File file)
{
    struct stat fd_stat = {0};
    int fstat_result = fstat(file, &fd_stat);
    Os_FileProperties props = {0};
    if(fstat_result != -1)
    {
        props = os_lnx_file_properties_from_stat(&fd_stat);
    }
    return props;
}

internal bool
os_dir_make(Str8 path)
{
    I32 result = mkdir((const char *)path.str, 0700);
    if (result == 0) {
        return true;
    } else {
        return false;
    }
}

// Exit
//=============================================================================

internal void
os_exit(I32 exit_code)
{
    exit(exit_code);
}

// Time
//=============================================================================

internal U32
os_now_unix(void)
{
    time_t t = time(0);
    return (U32)t;
}

internal U64
os_now_microsec(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    U64 result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
    return result;
}

internal void
os_sleep_microsec(U64 micosec)
{
    struct timespec ts = {
        .tv_sec = micosec / Million(1),
        .tv_nsec = (micosec % Million(1)) * Thousand(1),
    };
    nanosleep(&ts, NULL);
}

// OS Entry Points
//=============================================================================

int main(int argc, char *argv[])
{
    entry_point(argv);
}
