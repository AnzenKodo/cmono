#ifndef BASE_ALLOC_H
#define BASE_ALLOC_H

// Types
//====================================================================
typedef struct Alloc Alloc;
struct Alloc {
    void *(*alloc)(void *context, uint64_t size, uint64_t align);
    void (*free)(void *context, void *buffer, uint64_t size);
    void *context;
};

typedef struct Alloc_Arena Alloc_Arena;
struct Alloc_Arena {
    char *buffer;
    uint64_t size;
    uint64_t offset;
    uint64_t committed;
};


// Macros
//====================================================================

#define alloc_make(a, T, n) ((T *)((a).alloc)((a).context, sizeof(T)*n, AlignOf(T)))
#define alloc_free(a, p, n) ((a).free((a).context, p, sizeof(*p)*n))
#define alloc_arena_init(b, s) (Alloc){alloc_arena_push, alloc_arena_pop, &(Alloc_Arena){.buffer = b, .size = s}}

internal void *alloc_arena_push(void *context, uint64_t size, uint64_t align);
internal void alloc_arena_pop(void *context, void *buffer, uint64_t size);
internal void alloc_arena_reset(void *context);

#endif // BASE_ALLOC_H
