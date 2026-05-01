//~ ak: Generated Code
//=============================================================================

#include "./generated/ui.meta.h"

internal UI_Box ui_box()
{
    UI_Box box = ZERO_STRUCT;
    return box;
}

internal void ui_box_equip_display_string(UI_Box *box, Str8 string)
{
    box->string = string;
    box->flags |= UI_Box_Flag_HasDisplayString;
}

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

internal UI_Box *ui_root_from_state(UI_State *state)
{
    return state->root;
}

internal bool ui_box_is_nil(UI_Box *box)
{
    return box == 0 || box == &ui_box_nil;
}

