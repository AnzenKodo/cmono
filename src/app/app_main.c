// TODO(ak): make fmt printf's internal

//- ak: headers
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../draw/draw_include.h"
// #include "../font/font.h"
#include "./app.h"
#include "../game/game.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../render/render_include.c"
#include "../draw/draw_include.c"
// #include "../font/font.c"
#include "../game/game.c"

internal void base_main(void)
{
    // Program Init ===========================================================
    wl_init();
    Wl_Window window = wl_window_open(APP_NAME);
    // wl_window_border_set(window, false);
    render_init();
    Render_Handle window_equip = render_window_equip(window);
    game_init();
    
    // Program Loop ===========================================================
    while (!wl_should_exit())
    {
        Arena_Temp scratch = arena_scratch_begin(0, 0);
        wl_set_fps(120);
        Wl_Event_List events = wl_get_events(scratch.arena, 0);
        for(Wl_Event *event = events.first; event != 0; event = event->next)
        {
            if (event->kind == Wl_Event_Kind_WindowClose ||
                (BUILD_DEBUG && event->key == Wl_Key_Esc))
            {
                wl_exit();
                break;
            }
        }
        
        render_begin_frame();
        render_window_begin_frame(window, window_equip);
        
        draw_begin_frame();
        Rng2_F32 canvas_rect = wl_canvas_rect_from_window(window);
        Draw_Bucket *bucket = draw_bucket_make();
        DrawBucketScope(bucket)
        {
            dr_rect(canvas_rect, APP_BG_COLOR, 0.f, 0.f, 0.f);
            game_state->canvas_rect = canvas_rect;
            game_state->events = events;
            game_loop();
        }
        draw_submit_bucket(window, window_equip, bucket);
        render_window_end_frame(window, window_equip);
        render_end_frame();
        arena_scratch_end(scratch);
    }
    
    // Free Everything ========================================================
    render_window_unequip(window, window_equip);
    render_cleanup();
    wl_window_cleanup();
}
