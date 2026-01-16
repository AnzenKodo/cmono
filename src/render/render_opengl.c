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
    glBindAttribLocation(program_id, _Render_Opengl_Vertex_Loc_Position, "position");
    glBindAttribLocation(program_id, _Render_Opengl_Vertex_Loc_Texcoord, "texcoord");
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
    Unused(length);
    Unused(message);
    Unused(userParam);
    switch (id){
        case 131218:
        {
            // NOTE(aman.v): performance warning, do nothing.
        }break;
        default:
        {
            LogErrorLine(_os_core_state.log_context, "%s", message);
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

GLuint tex = 0;

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
    char *vs_source =
        "attribute vec4 position;\n"
        "attribute vec2 texcoord;\n"
        "varying vec2 v_tc;\n"
        "void main() {\n"
        "  gl_Position = position;\n"
        "  v_tc = texcoord;\n"
        "}\n";
    char *fs_source =
        "precision mediump float;\n"
        "varying vec2 v_tc;\n"
        "uniform sampler2D tex;\n"
        "void main() {\n"
        "  gl_FragColor = texture2D(tex, v_tc);\n"
        "}\n";
    char* vert_sources[] = {
        // shader_source_header,
        vs_source,
    };
    char* frag_sources[] = {
        // shader_source_header,
        fs_source,
    };
    uint32_t shader_id = _render_opengl_shader_load(vert_sources, ArrayLength(vert_sources), frag_sources, ArrayLength(frag_sources));
    // Texture Init ===========================================================
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLuint white_pixel = 0xffffffff;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white_pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

internal void render_deinit(void)
{
    _render_opengl_deinit();
}

internal void render(Img *img)
{
    glViewport(0, 0, wl_window_width_get(), wl_window_height_get());
    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Texture Render =========================================================
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
    // bind vertex input attribute
    glEnableVertexAttribArray(_Render_Opengl_Vertex_Loc_Position);
    glVertexAttribPointer(_Render_Opengl_Vertex_Loc_Position, 2, GL_FLOAT, false, 0, full_verts);
    glEnableVertexAttribArray(_Render_Opengl_Vertex_Loc_Texcoord);
    glVertexAttribPointer(_Render_Opengl_Vertex_Loc_Texcoord, 2, GL_FLOAT, false, 0, full_texcoords);
    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    GLenum format = _render_opengl_format_from_img_format(img->format);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, format, GL_UNSIGNED_BYTE, img->pixels);
    // draw
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // os internal opengl
    _render_opengl();
}

internal void render_shader_unload(uint32_t id)
{
    glDeleteProgram(id);
}
