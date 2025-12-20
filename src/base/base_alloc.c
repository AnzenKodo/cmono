// Alloc_Arena Allocator
//====================================================================

internal void *alloc_arena_push(void *context, uint64_t size, uint64_t align)
{
    void *result = 0;
    if (size)
    {
        Alloc_Arena *arena = (Alloc_Arena *)context;

        if (IsPow2(align))
        {
            uint64_t crr_ptr = IntFromPtr(arena->buffer) + arena->offset;
            uint64_t offset = AlignPow2(crr_ptr, align);
            offset -= IntFromPtr(arena->buffer);

            if (offset + size < arena->size)
            {
                arena->committed += size;
                result = (void *)(arena->buffer + offset);
                arena->offset = offset + size;
            }
        }
    }

    return result;
}

internal void alloc_arena_pop(void *context, void *buffer, uint64_t size)
{
    Alloc_Arena *arena = (Alloc_Arena *)context;
    uint64_t offset = IntFromPtr(buffer) - IntFromPtr(arena->buffer);
    if (size <= offset) {
        arena->offset = offset;
        arena->committed -= size;
    }
}

internal void alloc_arena_reset(void *context)
{
    Alloc_Arena *a = context;
    a->offset = 0;
    a->committed = 0;
}
