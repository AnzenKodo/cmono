#include "base/base_include.h"
#include "os/os_include.h"
#include "window_layer/window_layer_include.h"
#include "draw/draw_include.h"
#include "render/render_include.h"

#include "base/base_include.c"
#include "os/os_include.c"
#include "window_layer/window_layer_include.c"
#include "draw/draw_include.c"
#include "render/render_include.c"


typedef struct Game_State Game_State;
struct Game_State {
    F32 player_x;
    F32 player_y;
};

internal void entry_point(void) {
    // Program Init ===========================================================
    wl_window_open(str8("Scuttle"), 960, 540);
    U64 size = MB(20);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);
    Draw_Buffer draw_buffer = render_init(alloc);

    // Tilemap Init ===========================================================
    U32 tile_height = 30;
    U32 tile_width = 30;
    #define TILEMAP_COUNT_X 15
    #define TILEMAP_COUNT_Y 10
    I32 tilemap[TILEMAP_COUNT_Y][TILEMAP_COUNT_X] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 },
        { 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    // for (int i = 0; i < TILEMAP_COUNT_Y-1; ++i) {
    //     for (int j = 1; j < TILEMAP_COUNT_X; ++j) {
    //         tilemap[i][j] = (os_random_u32() % 10 == 0) ? 1 : 0;
    //     }
    // }

    // Player Init ============================================================
    F32 player_width = 0.75f * tile_width;
    F32 player_height = tile_height;

    // Gatae State Init =======================================================
    Game_State game_state = ZERO_STRUCT;
    game_state.player_x = 50;
    game_state.player_y = 260;

    // Game Loop ==============================================================
    while (!wl_should_window_close()) {
        wl_update_events();
        if (wl_is_key_pressed(Wl_Key_Esc))
        {
            wl_set_window_close();
        }

        render_begin();
        {
            draw_fill(draw_buffer, DRAW_RED);

            // Draw Tile Loop =================================================
            for (I32 row = 0; row < TILEMAP_COUNT_Y; ++row) {
                for (I32 col = 0; col < TILEMAP_COUNT_X; ++col) {
                    Draw_Rgba color = DRAW_BLUE;
                    if (tilemap[row][col] == 1) {
                        color = DRAW_YELLOW;
                    }

                    F32 rect_x = cast(F32)col * tile_width;
                    F32 rect_y = cast(F32)row * tile_height;
                    F32 rect_width  = rect_x + tile_width;
                    F32 rect_height = rect_y + tile_height;
                    draw_rect(draw_buffer, (Draw_Rect){
                        rect_x, rect_y, rect_width, rect_height
                    }, color);
                } // for x
            } // for y

            // Player Move ====================================================
            F32 player_left = game_state.player_x - player_width;
            F32 player_top = game_state.player_y - player_height;
            draw_rect(draw_buffer, (Draw_Rect) {
                player_left, player_top,
                player_left + player_width, player_top + player_height
            }, DRAW_PINK);
            draw_rect(draw_buffer, (Draw_Rect) {
                player_left, player_top,
                player_left + 2, player_top + 2
            }, DRAW_BLACK);
        }
        render_end();
    }

    // Free Everything ========================================================
    os_memory_free(buffer, size);
    wl_window_close();
}

int main(void)
{
    entry_point();
}
