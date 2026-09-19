// ak: headers
#include "std/base/base_include.h"
#include "std/os/os_include.h"
#include "std/window_layer/window_layer_include.h"
#include "std/render/render_include.h"
#include "std/font/font.h"
#include "std/draw/draw_include.h"
#include "std/audio/audio.h"
#include "./app.h"
#include "./generated/app.meta.h"

// ak: implementation
#include "std/base/base_include.c"
#include "std/os/os_include.c"
#include "std/window_layer/window_layer_include.c"
#include "std/render/render_include.c"
#include "std/font/font.c"
#include "std/draw/draw_include.c"
#include "std/audio/audio.c"
#include "./app.c"
#include "./generated/app.meta.c"

internal void base_main(void)
{
    // ak: Application Init ===================================================
    wl_init();
    Wl_Window window = wl_window_open(APP_NAME);
    render_init();
    font_init();
    audio_init(48000, 2);
    Render_Handle window_equip = render_window_equip(window);
    
    // ak: Application Loop ===================================================
    while (!wl_should_exit())
    {
        font_frame();
        Arena_Temp scratch = arena_scratch_begin(0, 0);
        wl_set_fps(15);
        Wl_Event_List events = wl_get_events(scratch.arena, 0);
        for (Wl_Event *event = events.first; event != 0; event = event->next)
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
        }
        draw_submit_bucket(window, window_equip, bucket);
        render_window_end_frame(window, window_equip);
        render_end_frame();
        arena_scratch_end(scratch);
    }
    
    // ak: Free Everything ====================================================
    render_window_unequip(window, window_equip);
    render_cleanup();
    audio_cleanup();
    wl_window_cleanup();
}
