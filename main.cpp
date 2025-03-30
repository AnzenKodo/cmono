// TODO: Refine Window Layer
// TODO: Make buffer pointer

#define ENABLE_WINDOW_LAYER
// #define RENDER_BACKEND RENDER_BACKEND_OPENGL
#define GL_GLEXT_PROTOTYPES

#include "ccore/base/base_include.h"
#include "ccore/core_draw.h"
#include "ccore/platform/platform_include.h"

#include "ccore/base/base_include.c"
#include "ccore/platform/platform_include.c"

#include "ccore/core_img.h"
#include "icon.h"

typedef struct Game_State Game_State;
struct Game_State {
    F32 player_x;
    F32 player_y;
};

typedef struct Tilemap Tilemap;
struct Tilemap {
    I32 count_x;
    I32 count_y;
    F32 upper_left_x;
    F32 upper_left_y;
    F32 tile_width;
    F32 tile_height;
    U32 *tiles;
};

fn U32 tilemap_get_tile_value_unchecked(Tilemap *tilemap, I32 tile_x, I32 tile_y)
{
    U32 tilemap_value = tilemap->tiles[tile_y*tilemap->count_x + tile_x];
    return tilemap_value;
}

fn Bool tilemap_is_empty(Tilemap *tilemap, F32 x, F32 y)
{
    Bool empty = false;

    I32 player_tile_x = truncat_f32_to_i32(
        (x - tilemap->upper_left_x) / tilemap->tile_width
    );
    I32 player_tile_y = truncat_f32_to_i32(
        (y - tilemap->upper_left_y) / tilemap->tile_height
    );

    if ((player_tile_x >= 0) && (player_tile_x < tilemap->count_x) &&
        (player_tile_y >= 0) && (player_tile_y < tilemap->count_y))
    {
        I32 tilemap_value = tilemap->tiles[player_tile_y*tilemap->count_x + player_tile_x];
        empty = (tilemap_value == 0);
    }

    return empty;
}

int main(void)
{
    TCTX tctx;
    tctx_init_and_equip(&tctx);
    Arena *arena = arena_alloc();

    wl_window_open(str8_lit("Scuttle"), 960, 540);
    wl_set_window_icon((const U8 *)SCUTTLEICON);

    // F32 monitor_refresh_rate = 60;
    // F32 game_update_hz = monitor_refresh_rate / 2.0f;
    // F32 target_sec_per_frame = 10.0f / game_update_hz;

    Draw_Buffer *buffer = render_init();

    // Game Start ======================================================
    Game_State *game_state = arena_push(arena, Game_State, 1);
    game_state->player_x = 50;
    game_state->player_y = 270;

#define TILEMAP_COUNT_X 15
#define TILEMAP_COUNT_Y 10
#define TILEMAP_NUM 2
    I32 tiles[TILEMAP_NUM][TILEMAP_COUNT_Y][TILEMAP_COUNT_X] = {
        {
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
            { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
            { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        },
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
            { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        }
    };

    Tilemap tilemaps[1];
    tilemaps[0].count_x = TILEMAP_COUNT_X;
    tilemaps[0].count_y = TILEMAP_COUNT_Y;
    tilemaps[0].upper_left_x = -30;
    tilemaps[0].upper_left_y = 0;
    tilemaps[0].tile_width = 30;
    tilemaps[0].tile_height = 30;
    tilemaps[0].tiles = (U32 *)tiles[0];

    tilemaps[1] = tilemaps[0];
    tilemaps[1].tiles = (U32 *)tiles[1];

    Tilemap *tilemap = &tilemaps[0];

    // =================================================================

    while (!wl_should_window_close()) {
        wl_enforce_frame_rate(60);
        F32 frame_rate = wl_get_current_frame_rate();
        wl_update_events();
        if (wl_is_key_pressed(Wl_Key_Esc))
        {
            wl_set_window_close();
        }

        render_begin();
        {
            draw_fill(buffer, ORANGE);

            for (I32 row = 0; row < tilemap->count_y; ++row) {
                for (I32 col = 0; col < tilemap->count_x; ++col) {
                    I32 tile_id =  tilemap_get_tile_value_unchecked(tilemap, col, row);

                    Rgba color = GREEN;
                    if (tile_id == 1) {
                        color = YELLOW;
                    }

                    F32 rect_x = tilemap->upper_left_x + cast(F32)col * tilemap->tile_width;
                    F32 rect_y = tilemap->upper_left_y + cast(F32)row * tilemap->tile_height;
                    F32 rect_width  = rect_x + tilemap->tile_width;
                    F32 rect_height = rect_y + tilemap->tile_height;
                    draw_rect(buffer, (Rect){
                        rect_x, rect_y, rect_width, rect_height
                    }, color);
                }
            }


            F32 d_player_x = 0.0f;
            F32 d_player_y = 0.0f;
            if (wl_is_key_pressed(Wl_Key_W)) {
                d_player_y = -1.0f;
            } else if (wl_is_key_pressed(Wl_Key_S)) {
                d_player_y = 1.0f;
            } else if (wl_is_key_pressed(Wl_Key_A)) {
                d_player_x = -1.0f;
            } else if (wl_is_key_pressed(Wl_Key_D)) {
                d_player_x = 1.0f;
            }
            d_player_x *= 210.0f;
            d_player_y *= 210.0f;

            F32 target_sec_per_frame = (1.0f /frame_rate);
            F32 new_player_x = game_state->player_x + target_sec_per_frame * d_player_x;
            F32 new_player_y = game_state->player_y + target_sec_per_frame * d_player_y;

            F32 player_width = 0.75f * tilemap->tile_width;
            F32 player_height = tilemap->tile_height;

            if (
                tilemap_is_empty(tilemap, new_player_x, new_player_y) &&
                tilemap_is_empty(tilemap, new_player_x + 0.5f*player_width, new_player_y) &&
                tilemap_is_empty(tilemap, new_player_x - 0.5f*player_width, new_player_y)
            ) {
                game_state->player_x = new_player_x;
                game_state->player_y = new_player_y;
            }

            F32 player_left = game_state->player_x - (0.5f * player_width);;
            F32 player_top = game_state->player_y - player_height + 1;

            draw_rect(buffer, (Rect) {
                player_left, player_top,
                player_left + player_width, player_top + player_height
            }, BLUE);

            // Draw tile dot
            draw_rect(buffer, (Rect) {
                new_player_x, new_player_y,
                new_player_x + 2, new_player_y + 2
            }, BLACK);
        }
        render_end();
    }

    render_close();
    wl_window_close();
    arena_free(arena);
    tctx_release();
    return 0;
}
