#ifndef BASE_ALLOC_H
#define BASE_ALLOC_H

// Arena
//=============================================================================

typedef struct Arena Arena;
struct Arena {
    char   *buffer;
    size_t reserve_size;
    size_t commit_size;
    size_t commit_pos;
    size_t pos;
};

#define ARENA_HEADER_SIZE (sizeof(Arena))
#define arena_push(a, T, n) ((T *)(_arena_push(a, sizeof(T)*(n), AlignOf(T), true)))
#define arena_push_nz(a, T, n) ((T *)(_arena_push(a, sizeof(T)*(n), AlignOf(T), false)))

internal Arena *arena_alloc(size_t reserve_size, size_t commit_size);
internal void arena_free(Arena *arena);
internal void *_arena_push(Arena *arena, size_t size, size_t align, bool fill_zero);
internal void arena_pop(Arena *arena, size_t size);
internal void arena_pop_to(Arena *arena, size_t pos);
internal void arena_clear(Arena *arena);

// Temp Arena
// ============================================================================

typedef struct Arena_Temp Arena_Temp;
struct Arena_Temp
{
    Arena *arena;
    size_t pos;
};

internal Arena_Temp arena_temp_begin(Arena *arena);
internal void arena_temp_end(Arena_Temp temp);

// Scratch Arena
// ============================================================================

thread_static Arena *_arena_scratch_global[2] = { NULL, NULL };

#define arena_scratch_begin(conflicts, count) arena_temp_begin(arena_scratch((conflicts), (count)))
#define arena_scratch_end(scratch) arena_temp_end(scratch)

internal Arena *arena_scratch(Arena **conflicts, size_t length);

#endif // BASE_ALLOC_H
