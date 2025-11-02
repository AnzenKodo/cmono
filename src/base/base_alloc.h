#ifndef BASE_ALLOC_H
#define BASE_ALLOC_H

// Types
//====================================================================
typedef struct Alloc Alloc;
struct Alloc {
    void *(*alloc)(void *context, U64 size, U64 align);
    void (*free)(void *context, void *buffer, U64 size);
    void *context;
};

typedef struct Alloc_Arena Alloc_Arena;
struct Alloc_Arena {
    char *buffer;
    U64 size;
    U64 offset;
    U64 committed;
};


// Macros
//====================================================================

#define alloc_make(a, T, n) (Cast(T *)((a).alloc((a).context, sizeof(T)*n, AlignOf(T))))
#define alloc_free(a, p, n) ((a).free((a).context, p, sizeof(*p)*n))
#define alloc_arena_init(b, s) (Alloc){alloc_arena_push, alloc_arena_pop, &(Alloc_Arena){.buffer = b, .size = s}}

internal void *alloc_arena_push(void *context, U64 size, U64 align);
internal void alloc_arena_pop(void *context, void *buffer, U64 size);
internal void alloc_arena_reset(void *context);

#endif // BASE_ALLOC_H
