// NOTE TODO
#include "../base/base_include.h"
#include "../os/os_include.h"
#include "../window_layer/window_layer_include.h"
#include "../render/render_include.h"
#include "./cope_core.h"

#include "../base/base_include.c"
#include "../os/os_include.c"
#include "../window_layer/window_layer_include.c"
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
    fmt_println("   "PROGRAM_VERSION);
}

internal void entry_point(void)
{
    uint64_t size = MB(10);
    void *buffer = os_memory_alloc(size);
    Alloc alloc = alloc_arena_init(buffer, size);

    Flags_Context context = flags_init(alloc);
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
        fmt_print("v"PROGRAM_VERSION);
        os_exit(0);
    }

    // uint32_t memory_size = (width * height) * 4;
    // uint8_t *memory = alloc_make(alloc, uint8_t, memory_size);
    // uint32_t tex = render_tex2d_load_raw(memory, width, height);

    // Program Init ===========================================================
    int width = 100;
    int height = 200;
    wl_window_open(str8(PROGRAM_NAME), width, height);
    // wl_window_border_set(false);
    wl_window_pos_set(0, 0);
    render_init();

    // glViewport(0, 0, width, height);
    /* Red texture (full background) */
    GLuint tex_red;
    glGenTextures(1, &tex_red);
    glBindTexture(GL_TEXTURE_2D, tex_red);
    unsigned char red_pixel[] = {255, 0, 0};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, red_pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    /* Blue texture (smaller overlay) */
    GLuint tex_blue;
    glGenTextures(1, &tex_blue);
    glBindTexture(GL_TEXTURE_2D, tex_blue);
    unsigned char blue_pixel[3] = {0, 0, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, blue_pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const char *vs_source =
        "attribute vec4 pos;\n"
        "attribute vec2 tc;\n"
        "varying vec2 v_tc;\n"
        "void main() {\n"
        "  gl_Position = pos;\n"
        "  v_tc = tc;\n"
        "}\n";
    static const GLfloat full_verts[8] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f,  1.0f,
        1.0f,  1.0f
    };
    static const GLfloat full_texcoords[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_source, NULL);
    glCompileShader(vs);
    const char *fs_source =
        "precision mediump float;\n"
        "varying vec2 v_tc;\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
        "  gl_FragColor = texture2D(tex, v_tc);\n"
        "}\n";
    static const GLfloat small_verts[8] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f,  0.5f,
        0.5f,  0.5f
    };
    static const GLfloat small_texcoords[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_source, NULL);
    glCompileShader(fs);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glBindAttribLocation(prog, 0, "pos");
    glBindAttribLocation(prog, 1, "tc");
    glLinkProgram(prog);
    glUseProgram(prog);
    GLint tex_loc = glGetUniformLocation(prog, "tex");
    glUniform1i(tex_loc, 0);
    while (!wl_should_window_close())
    {
        wl_set_fps(60);
        wl_update_events();
        if ((wl_is_key_pressed(Wl_Key_Esc)) ||
            wl_is_event_happen(Wl_EventType_WindowClose)
        ) {
            wl_set_window_close();
        }
        render_begin();
        {
            glClear(GL_COLOR_BUFFER_BIT);

            /* Draw full-screen red background */
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_red);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, full_verts);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, full_texcoords);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            /* Draw smaller blue square in the center */
            glBindTexture(GL_TEXTURE_2D, tex_blue);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, small_verts);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, small_texcoords);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        _render_opengl_end();
    }

    // Free Everything ========================================================
    wl_window_close();
    os_memory_free(buffer, size);
}
