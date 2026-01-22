// OpenGL Helper functions
//=============================================================================

internal uint32_t _render_opengl_shader_compile(char **source, uint32_t source_num, GLenum type, uint32_t program_id)
{
    GLuint shader_id = glCreateShader(type);
#if OS_LINUX
    glShaderSource(shader_id, source_num, (const char * const*)source, NULL);
#else
    glShaderSource(shader_id, source_num, source, NULL);
#endif
    glCompileShader(shader_id);
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char log[512];
        glGetShaderInfoLog(shader_id, 512, NULL, log);
        LogErrorLine(_os_core_state.log_context, "Shader program compile failed.");
    }
    glAttachShader(program_id, shader_id);
    return shader_id;
}

internal uint32_t _render_opengl_shader_load(char **vert_sources, uint32_t vert_source_num, char **frag_sources, uint32_t frag_source_num)
{
    uint32_t program_id = glCreateProgram();
    uint32_t vert_id = _render_opengl_shader_compile(vert_sources, vert_source_num, GL_VERTEX_SHADER, program_id);
    uint32_t frag_id = _render_opengl_shader_compile(frag_sources, frag_source_num, GL_FRAGMENT_SHADER, program_id);
    glBindAttribLocation(program_id, _Render_Opengl_Vertex_Loc_Pos, "pos");
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if (!status)
    {
        char log[512];
        glGetProgramInfoLog(program_id, 512, NULL, log);
        LogErrorLine(_os_core_state.log_context, "%s", log);
    }
    glUseProgram(program_id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    return program_id;
}

internal void _render_opengl_error_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
    Unused(source);
    Unused(type);
    Unused(severity);
    Unused(userParam);
    switch (id){
        case 131218:
        {
            // NOTE(aman.v): performance warning, do nothing.
        }break;
        default:
        {
            LogErrorLine(_os_core_state.log_context, "%.*s", length, message);
        }break;
    }
}

internal GLenum _render_opengl_format_from_img_format(Img_Format format)
{
    GLenum result = 0;
    switch (format)
    {
        case Img_Format_R8:     { result = GL_RED;  }break;
        case Img_Format_RG8:    { result = GL_RG;   }break;
        case Img_Format_RGBA8:  { result = GL_RGBA; }break;
        case Img_Format_BGRA8:  { result = GL_BGRA; }break;
        case Img_Format_R16:    { result = GL_RED;  }break;
        case Img_Format_RGBA16: { result = GL_RGBA; }break;
        case Img_Format_R32:    { result = GL_RED;  }break;
        case Img_Format_RG32:   { result = GL_RG;   }break;
        case Img_Format_RGBA32: { result = GL_RGBA; }break;
    }
    return result;
};

// Core functions
//=============================================================================
internal void render_init(void)
{
    _render_opengl_init();
#define X(name, r, p) name = (name##_FunctionType *)(void*)_render_opengl_load_procedure(#name);
    RenderOpenglXMacro
#undef X
#ifdef BUILD_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    if (glDebugMessageControl){
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, true);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW,          0, 0, true);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM,       0, 0, true);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH,         0, 0, true);
    }
    if (glDebugMessageCallback){
        glDebugMessageCallback(_render_opengl_error_callback, 0);
    }
#endif
    // Shader Init ============================================================
    const char *vs_source =
        "#version 330 core\n"
        "layout(location = 0) in vec2 pos;\n"
        "uniform vec2 uResolution;\n"
        "uniform vec2 uOffset;\n"
        "uniform vec2 uSize;\n"
        "void main()\n"
        "{\n"
        "    vec2 rect_pos = pos * uSize + uOffset;\n"
        "    vec2 clip = (rect_pos / uResolution) * 2.0 - 1.0;\n"
        "    gl_Position = vec4(clip.x, -clip.y, 0.0, 1.0);\n"
        "}\n";
    const char *fs_source =
        "#version 330 core\n"
        "uniform vec4 uColor;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = uColor;\n"
        "}\n";
    char* vert_sources[] = {
        vs_source,
    };
    char* frag_sources[] = {
        fs_source,
    };
    _render_opengl_state.shader = _render_opengl_shader_load(vert_sources, ArrayLength(vert_sources), frag_sources, ArrayLength(frag_sources));
    // generate vertex storage
    glGenVertexArrays(1, &_render_opengl_state.vao);
    glGenBuffers(1, &_render_opengl_state.vbo);
    // bind vertex input attribute
    glBindVertexArray(_render_opengl_state.vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state.vbo);
        float vertices[] = {
            0.0f, 0.0f,   // top-left
            0.0f, 1.0f,   // bottom-left
            1.0f, 0.0f,   // top-right
            1.0f, 1.0f    // bottom-right
        };
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(_Render_Opengl_Vertex_Loc_Pos);
        glVertexAttribPointer(_Render_Opengl_Vertex_Loc_Pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    }
    glBindVertexArray(0);
}

internal void render_deinit(void)
{
    _render_opengl_deinit();
}

internal void render(Draw_List *list)
{
    glViewport(0, 0, wl_window_width_get(), wl_window_height_get());
    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // start shader program
    glUseProgram(_render_opengl_state.shader);
    glBindVertexArray(_render_opengl_state.vao);
    for (Draw_Node *node = list->first; node != NULL; node = node->next)
    {
        switch (node->type)
        {
            case Draw_Type_Rect:
            {
            } break;
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
    // os internal opengl
    _render_opengl();
}

internal void render_shader_unload(uint32_t id)
{
    glDeleteProgram(id);
}
