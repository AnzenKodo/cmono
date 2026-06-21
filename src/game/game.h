#ifndef GAME_H
#define GAME_H

// ak: Types
//=============================================================================

typedef struct Game_State Game_State;
struct Game_State
{
    Rng2_F32 canvas_rect;
    size_t cell_size;
    Vec2_I32 cells;
    Wl_Event_List events;
};

// ak: Globals
//=============================================================================

global Game_State *game_state = NULL;

#endif // GAME_H
