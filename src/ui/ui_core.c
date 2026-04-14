//~ ak: Generated Code
//=============================================================================

#include "./generated/ui.meta.h"

internal UI_Box ui_box()
{
    UI_Box box = ZERO_STRUCT;
    // box.font =
    return box;
}

typedef struct UI_State UI_State;
struct UI_State
{
    Arena *arena;
    struct
    {
        UI_Parent_Node *top;
        UI_Box *bottom_val;
        UI_Parent_Node *free;
        bool auto_pop;
        size_t gen;
    } parent_stack;
    UI_DeclStackNils;
    UI_Parent_Node parent_nil_stack_top;
};

global UI_State *ui_state = 0;

internal UI_State *ui_state_alloc(Arena *arena)
{
    UI_State *state = arena_push(arena, UI_State, 1);
    state->arena = arena;
    UI_InitStackNils(state);
    UI_InitStacks(state);
    return state;
}

internal void ui_state_select(UI_State *state)
{
    ui_state = state;
}

