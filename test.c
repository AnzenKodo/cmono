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

typedef struct Game_State Gamge_State;
struct Game_State {
    F32 player_x;
    F32 player_y;
};

#define TILE_SIZE 30.0f
int main(void)
{
    TCTX tctx;
    tctx_init_and_equip(&tctx);
    Arena *arena = arena_alloc();

    wl_window_open(str8_lit("Scuttle"), 960, 540);
    wl_set_window_icon((const U8 *)SCUTTLEICON);

    Draw_Buffer *buffer = render_init();

    int tilemap[10][10] = {
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {0, 1, 1, 0, 1, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {0, 1, 1, 0, 1, 1, 0, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    while (!wl_should_window_close()) {
        wl_enforce_frame_rate(100);
        F32 frame_rate = wl_get_current_frame_rate();
        wl_update_events();
        if (wl_is_key_pressed(Wl_Key_Esc))
        {
            wl_set_window_close();
        }

        render_begin();
        {
            draw_fill(buffer, WHITE);
            for (int y = 0; y < 10; y++)
            {
                for (int x = 0; x < 10; x++)
                {
                    I32 tile_id = tilemap[y][x];

                    Rgba color = GREEN;
                    if (tile_id == 1)
                    {
                        color = YELLOW;
                    }
                    F32 min_x = (cast(F32)x)*TILE_SIZE;
                    F32 min_y = (cast(F32)y)*TILE_SIZE;
                    F32 max_x = min_x + TILE_SIZE;
                    F32 max_y = min_y + TILE_SIZE;
                    draw_rect(buffer, (Rect){
                        min_x, min_y, max_x, max_y
                    }, color);
                }
            }
        }
        render_end();
    }


    render_close();
    wl_window_close();
    arena_free(arena);
    tctx_release();
    return 0;
}
