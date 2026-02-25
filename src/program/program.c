// TODO(ak): embead font in exe
// TODO(ak): setup Github Action
// TODO(ak): change program version
#include "./program.h"
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "../font/font.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
#include "../font/font.c"
#include "../render/render_include.c"

internal void print_help_message(Flags_Context *context)
{
    term_style_start(TERM_UNDERLINE);
    fmt_println("USAGE:");
    term_style_end();
    Str8 *program_name = os_program_path_get();
    fmt_printfln("   %s [OPTIONS] <shader_file_name.frag>", program_name->cstr);
    term_style_start(TERM_UNDERLINE);
    fmt_println("OPTIONS:");
    term_style_end();
    flags_print_help(context);
    term_style_start(TERM_UNDERLINE);
    fmt_println("VERSION:");
    term_style_end();
    fmt_println("   "PROG_VERSION);
}

internal void prog_entry_point(void)
{
    Arena *arena = arena_alloc(MB(10), MB(1));
    // Command Line ===========================================================
    {
        Arena_Temp temp = arena_temp_begin(arena);
        Flags_Context context = flags_begin(temp.arena);
        Flags_Option *option = NULL;
        bool help = false;
        option = flags_option_bool(&context, str8("help"), &help, help, str8("Print help message"));
        flags_add_option_shortname(option, str8("h"));
        bool version = false;
        option = flags_option_bool(&context, str8("version"), &version, version, str8("Print version message"));
        flags_add_option_shortname(option, str8("v"));
        flags_add_color_flags(&context);
        Str8Array *args = os_args_get();
        if (!flags_parse(&context, args))
        {
            flags_print_error(&context);
            print_help_message(&context);
            os_exit(1);
        }
        if (help)
        {
            print_help_message(&context);
            os_exit(0);
        }
        if (version)
        {
            fmt_print("v"PROG_VERSION);
            os_exit(0);
        }
        arena_temp_end(temp);
    }
    // Program Init ===========================================================
    unsigned int width = 150;
    unsigned int height = 300;
    unsigned int row_height = 25;
    wl_window_open(str8(PROG_NAME), width, height);
    wl_window_border_set(false);
    wl_window_pos_set(wl_display_width_get()-width-30, 0);
    render_init();
    Font font = font_load(str8("./assets/font/VendSans-Regular.ttf"), width, height, arena);
    Render_Draw_List list = ZERO_STRUCT;
    while (!wl_should_window_close())
    {
        Arena_Temp temp = arena_temp_begin(arena);
        wl_set_fps(60);
        wl_update_events();
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose))
        {
            wl_set_window_close();
        }
        render_draw_rect_push(temp.arena, &list, (Vec4_F32){ 0, 0, (float)width, (float)height }, PROG_BACKGROUND_COLOR);
        Vec4_F32 size = (Vec4_F32){ 0.0, 0.0, (float)width, (float)row_height };
        Vec4_F32 padding = (Vec4_F32){ 3.5f, 0.0f, 3.5f, 0.0f };
        render_draw_rect_text_push(temp.arena, &list, &font, str8("Hello"), size, padding, PROG_FOREGROUND_COLOR, PROG_BACKGROUND_COLOR);
        render(&list);
        arena_temp_end(temp);
    }

    // Free Everything ========================================================
    wl_window_close();
    render_deinit();
}
