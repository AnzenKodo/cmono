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

    // Program Init ===========================================================
    int width = 100;
    int height = 200;
    wl_window_open(str8(PROGRAM_NAME), (Vec2_U64){width, height});
    wl_window_border_set(false);
    wl_window_pos_set((Vec2_U64){0, 0});
    render_init();
    char *vert_source = "layout(location = 0) in vec3 aPos;\n"
        "layout(location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
        "    TexCoord = aTexCoord;  // Pass UV to fragment shader\n"
        "}\n";
    char *frag_source = "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D myTexture;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(myTexture, TexCoord);\n"
        "}\n";
    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    uint32_t id = render_shader_load(str8(vert_source), str8(frag_source));
    GLint positionAttrib = glGetAttribLocation(id, "position");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    uint32_t memory_size = (width * height) * 4;
    uint8_t *memory = alloc_make(alloc, uint8_t, memory_size);
    // memory[0] = 1;
    // memory[1] = 2;
    // memory[2] = 2;
    uint32_t tex = render_tex2d_load_raw(memory, width, height);

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
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex);
            GLint texLocation = glGetUniformLocation(id, "myTexture");
            glUniform1i(texLocation, 0);  // 0 = GL_TEXTURE0
        }
        render_end();
    }

    // Free Everything ========================================================
    render_deinit();
    wl_window_close();
    os_memory_free(buffer, size);
}
