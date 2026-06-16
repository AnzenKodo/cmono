// TODO(ak): update event handling
// TODO(ak): make fmt printf's internal

//- ak: headers
#include "./app.h"
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../draw/draw.h"
// #include "../font/font.h"
// #include "../ui/ui_include.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../render/render_include.c"
#include "../draw/draw.c"
// #include "../font/font.c"
// #include "../ui/ui_include.c"

internal void base_main(void)
{
    // Program Init ===========================================================
    unsigned int width = 150;
    unsigned int height = 300;
    wl_init();
    Wl_Window window = wl_window_open(str8(APP_NAME), width, height);
    // wl_window_border_set(window, false);
    wl_window_pos_set(window, wl_display_width_get()-width-30, 0);
    render_init();
    Render_Handle window_equip = render_window_equip(window);
    
    // Program Loop ===========================================================
    while (!wl_should_exit())
    {
        wl_set_fps(60);
        wl_update_events();
        
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose))
        {
            wl_exit();
        }
        
        render_begin_frame();
        render_window_begin_frame(window, window_equip);
        
        draw_begin_frame();
        Rng2_F32 client_rect = wl_canvas_rect_from_window(window);
        Draw_Bucket *bucket = draw_bucket_make();
        DrawBucketScope(bucket)
        {
            dr_rect(client_rect, (Vec4_F32){ 0.08f, 0.08f, 0.11f, 1.f }, 0.f, 0.f, 0.f);
        }
        draw_submit_bucket(window, window_equip, bucket);
        render_window_end_frame(window, window_equip);
        render_end_frame();
    }
    
    // Free Everything ========================================================
    render_window_unequip(window, window_equip);
    render_cleanup();
    wl_window_close();
}
