// TODO(ak): make fmt printf's internals use str8

// ak: headers
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../font/font.h"
#include "../draw/draw_include.h"
#include "./app.h"
#include "../game/game.hpp"

// ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../render/render_include.c"
#include "../font/font.c"
#include "../draw/draw_include.c"
#include "../game/game.cpp"

internal void base_main(void)
{
    // ak: Application Init ===================================================
    wl_init();
    Wl_Window window = wl_window_open(APP_NAME);
    // wl_window_border_set(window, false);
    render_init();
    font_init();
    Render_Handle window_equip = render_window_equip(window);
    game_init();
    
    Str8 font_path = str8("/usr/share/fonts/TTF/DejaVuSans.ttf");
    Font_Tag tag = font_tag_from_path(font_path);
    if(font_tag_match(tag, font_tag_zero()))
    {
    }
    
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
            draw_rect(canvas_rect, game_state->color.background, 0.f, 0.f, 0.f);
            game_state->rect.canvas = canvas_rect;
            game_state->event.window_resize = false;
            for(Wl_Event *event = events.first; event != 0; event = event->next)
            {
                switch (event->kind)
                {
                    default: break;
                    case Wl_Event_Kind_WindowResize: { game_state->event.window_resize = true; } break;
                    case Wl_Event_Kind_Press:
                    {
                        switch (event->key) {
                            default: break;
                            case Wl_Key_Up:
                            {
                                if (game_state->event.direction != Game_Direction_Down)
                                {
                                    game_state->event.direction = Game_Direction_Up;
                                }
                            } break;
                            case Wl_Key_Down:
                            {
                                if (game_state->event.direction != Game_Direction_Up)
                                {
                                    game_state->event.direction = Game_Direction_Down;
                                }
                            } break;
                            case Wl_Key_Left:
                            {
                                if (game_state->event.direction != Game_Direction_Right)
                                {
                                    game_state->event.direction = Game_Direction_Left;
                                }
                            } break;
                            case Wl_Key_Right:
                            {
                                if (game_state->event.direction != Game_Direction_Left)
                                {
                                    game_state->event.direction = Game_Direction_Right;
                                }
                            } break;
                        }
                    }
                    break;
                }
            }
            game_loop();
            draw_text(tag, 8.f, 0.f, 4.f, Font_Raster_Flag_Smooth, (Vec2_F32){ 20.f, 20.f }, (Vec4_F32){ 0.80f, 0.80f, 0.80f, 1.f }, str8("Hello World"));
        }
        draw_submit_bucket(window, window_equip, bucket);
        render_window_end_frame(window, window_equip);
        render_end_frame();
        arena_scratch_end(scratch);
    }
    
    // ak: Free Everything ====================================================
    render_window_unequip(window, window_equip);
    render_cleanup();
    wl_window_cleanup();
}
