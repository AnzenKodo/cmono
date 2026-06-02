// TODO(ak): add multi-window support
// TODO(ak): update event handling
// TODO(ak): make flags state based
// TODO(ak): make fmt printf's internal

//~ ak: Includes
//=============================================================================

//- ak: headers
#include "./app.h"
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../font/font.h"
#include "../ui/ui_include.h"

//- ak: implementation
#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../font/font.c"
#include "../render/render_include.c"
#include "../ui/ui_include.c"

internal void base_main(void)
{
    Arena *arena = arena_alloc();
    
    // Program Init ===========================================================
    unsigned int width = 150;
    unsigned int height = 300;
    // unsigned int row_height = 25;
    wl_window_open(str8(APP_NAME), width, height);
    wl_window_border_set(false);
    wl_window_pos_set(wl_display_width_get()-width-30, 0);
    render_init();
    // Font font = font_load(str8("./assets/font/VendSans-Regular.ttf"), width, height, arena);
    UI_State *state = ui_state_alloc(arena);
    
    // Program Loop ===========================================================
    while (!wl_should_window_close())
    {
        Arena_Temp scratch = arena_scratch_begin(0, 0);
        Render_Draw_List list = ZERO_STRUCT;
        wl_set_fps(60);
        wl_update_events();
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose))
        {
            wl_set_window_close();
        }
        
        // ak: build ui
        ui_build_begin(state);
        {
            ui_pref_width_push(ui_em(20.f, 1.f));
            ui_pref_height_push(ui_em(4.f, 1.f));
            {
        //         ui_labelf("Here is a panel.");
        //         if (ui_buttonf("Hello World").clicked_left)
                {
        //             ui_buttonf("Hellow World");
                }
            }
        }
        ui_build_end();
        
        // ak: render ui
        UI_Box_Rec rec;
        for(UI_Box *box = ui_root_from_state(state); !ui_box_is_nil(box); box = rec.next)
        {
            rec = ui_box_rec_df_post(box, &ui_box_nil);
            if (box->flags & UI_Box_Flag_DrawBackground)
            {
        //         Vec2_F32 rect = box->rect;
                render_draw_rect_push(scratch.arena, &list, (Vec4_F32){ 0, 0, box->fixed_size.x, box->fixed_size.y }, APP_BACKGROUND_COLOR);
            }
        }
        render(&list);
        arena_temp_end(scratch);
    }
    
    // Free Everything ========================================================
    wl_window_close();
    render_deinit();
}
