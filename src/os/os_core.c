internal void *
os_memory_alloc(U64 size)
{
    void *result = os_memory_create(size);
    os_memory_commit(result, size);
    return result;
}

