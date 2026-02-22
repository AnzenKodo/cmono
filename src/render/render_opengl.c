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
            // NOTE(ak): performance warning, do nothing.
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
        default:  { result = GL_RGBA; }break;
    }
    return result;
};

// Core functions
//=============================================================================

global Font temp_font = {0};
global GLuint temp_texture = {0};
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
    char *vs_source =
        "layout(location = 0) in vec2 pos;\n"
        "uniform vec2 u_offset;\n"
        "uniform vec2 u_size;\n"
        "uniform vec2 u_uv_offset;\n"
        "uniform vec2 u_uv_size;\n"
        "uniform vec2 u_resolution;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "    vec2 rect_pos = pos * u_size + u_offset;\n"
        "    vec2 clip = (rect_pos / u_resolution) * 2.0 - 1.0;\n"
        "    gl_Position = vec4(clip.x, -clip.y, 0.0, 1.0);\n"
        "    v_uv = pos * u_uv_size + u_uv_offset;\n"
        "}\n";
    char *fs_source =
        "uniform vec4 u_color;\n"
        "uniform sampler2D u_tex;\n"
        "in vec2 v_uv;\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    float alpha = texture(u_tex, v_uv).r;\n"
        "    FragColor = vec4(u_color.rgb, u_color.a * alpha);\n"
        "}\n";
    char* vert_sources[] = {
        shader_source_header,
        vs_source,
    };
    char* frag_sources[] = {
        shader_source_header,
        fs_source,
    };
    _render_opengl_state.shader = _render_opengl_shader_load(vert_sources, ArrayLength(vert_sources), frag_sources, ArrayLength(frag_sources));
    // ak: Create 1x1 white texture for solid color fills
    glGenTextures(1, &_render_opengl_state.default_texture);
    glBindTexture(GL_TEXTURE_2D, _render_opengl_state.default_texture);
    uint8_t white[4] = {255, 255, 255, 255};
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    // ak: Generate vertex storage (unit quad)
    glGenVertexArrays(1, &_render_opengl_state.vao);
    glGenBuffers(1, &_render_opengl_state.vbo);
    glBindVertexArray(_render_opengl_state.vao);
    {
        glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state.vbo);
        float vertices[] = {
            0.0f, 0.0f,   // top-left
            0.0f, 1.0f,   // bottom-left
            1.0f, 0.0f,   // top-right
            1.0f, 1.0f    // bottom-right
        };
        glBufferData(GL_ARRAY_BUFFER, KB(64), 0, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glEnableVertexAttribArray(_Render_Opengl_Vertex_Loc_Pos);
        glVertexAttribPointer(_Render_Opengl_Vertex_Loc_Pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(0);
    glGenTextures(1, &temp_texture);
    glBindTexture(GL_TEXTURE_2D, temp_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, temp_font.atlas_width, temp_font.atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, temp_font.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

internal void render_deinit(void)
{
    glDeleteVertexArrays(1, &_render_opengl_state.vao);
    glDeleteBuffers(1, &_render_opengl_state.vbo);
    _render_opengl_deinit();
}

internal void render(Draw_List *list)
{
    uint32_t win_width = wl_window_width_get();
    uint32_t win_height = wl_window_height_get();
    glViewport(0, 0, win_width, win_height);
    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // start shader program
    glUseProgram(_render_opengl_state.shader);
    glBindVertexArray(_render_opengl_state.vao);
    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_resolution"), (GLfloat)win_width, (GLfloat)win_height);
    for (Draw_Node *node = list->first; node != NULL; node = node->next)
    {
        switch (node->type)
        {
            case Draw_Type_Rect:
            {
                glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state.vbo);
                // for (){
                //     glBufferSubData(GL_ARRAY_BUFFER, off, batch_n->v.byte_count, batch_n->v.v);
                // }
                Draw_Rect rect = node->param_rect;
                float x = rect.dst.x;
                float y = rect.dst.y;
                float width = rect.dst.z;
                float height = rect.dst.w;
                Vec4_F32 color = rect.color;
                // Fill
                // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
                glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_offset"), x, y);
                glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_size"),   width, height);
                glUniform4f(glGetUniformLocation(_render_opengl_state.shader, "u_color"),  color.x, color.y, color.z, color.w);
                glBindTexture(GL_TEXTURE_2D, _render_opengl_state.default_texture);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            } break;
        }

    }

    float cx = 20.0f;
    float cy = 20.0f;
    char *text = "Hello World \n New Texes";
    glBindTexture(GL_TEXTURE_2D, temp_texture);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, temp_texture);
    for (char *c = text; *c; ++c)
    {
        if (*c < 32 || *c > 126)
        {
            if (*c == '\n')
            {
                cy += 24.0f;
                cx = 20.0f;
            }
            continue;
        }
        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(temp_font.data, temp_font.atlas_width, temp_font.atlas_height, *c - 32, &cx, &cy, &q, true);
        glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_offset"),    q.x0, q.y0);
        glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_size"),      q.x1-q.x0, q.y1-q.y0);
        glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_uv_offset"), q.s0, q.t0);
        glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_uv_size"),   q.s1-q.s0, q.t1-q.t0);
        glUniform4f(glGetUniformLocation(_render_opengl_state.shader, "u_color"),     2.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    // os internal opengl
    _render_opengl();
}
