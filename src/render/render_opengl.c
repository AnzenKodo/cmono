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

// Core functions
//=============================================================================

internal void render_init(void)
{
    _render_opengl_init();

#define X(name, r, p) name = (name##_FunctionType *)(void*)_render_opengl_load_procedure(#name);
    RenderOpenglXMacro
#undef X

    glGenVertexArrays(1, &_render_opengl_state.vertex_arrays);
    glBindVertexArray(_render_opengl_state.vertex_arrays);

    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    _render_opengl_state.array_buffer_size = sizeof(vertices);
    glGenBuffers(1, &_render_opengl_state.array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state.array_buffer);
    glBufferData(GL_ARRAY_BUFFER, _render_opengl_state.array_buffer_size, vertices, GL_STATIC_DRAW);
}

internal void render_deinit(void)
{
    _render_opengl_deinit();
    glDeleteVertexArrays(sizeof(&_render_opengl_state.vertex_arrays), &_render_opengl_state.vertex_arrays);
    glDeleteBuffers(_render_opengl_state.array_buffer_size, &_render_opengl_state.array_buffer);
}

internal void render_begin(void)
{
    _render_opengl_begin();
}

internal void render_end(void)
{
    _render_opengl_end();
}

internal uint32_t render_shader_load_multi(char **vert_sources, uint32_t vert_source_num, char **frag_sources, uint32_t frag_source_num)
{
    uint32_t program_id = glCreateProgram();
    uint32_t vert_id = _render_opengl_shader_compile(vert_sources, vert_source_num, GL_VERTEX_SHADER, program_id);
    uint32_t frag_id = _render_opengl_shader_compile(frag_sources, frag_source_num, GL_FRAGMENT_SHADER, program_id);
    glLinkProgram(program_id);
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

internal uint32_t render_shader_load(Str8 vert_source, Str8 frag_source)
{
    char* vert_sources[] = {
        shader_source_header,
        (char *)vert_source.cstr
    };
    char* frag_sources[] = {
        shader_source_header,
        (char *)frag_source.cstr
    };
    return render_shader_load_multi(vert_sources, 2, frag_sources, 2);
}

internal void render_shader_unload(uint32_t id)
{
    glDeleteProgram(id);
}

internal uint32_t render_shader_get_value(uint32_t shader_id, Str8 name)
{
    return glGetUniformLocation(shader_id, (char *)name.cstr);
}

internal void render_shader_set_value_vec(uint32_t value_index, const void *value, Render_Shader type, int32_t count)
{
    switch (type)
    {
        case Render_Shader_Float:  glUniform1fv(value_index, count, (float *)value); break;
        case Render_Shader_Vec2:   glUniform2fv(value_index, count, (float *)value); break;
        case Render_Shader_Vec3:   glUniform3fv(value_index, count, (float *)value); break;
        case Render_Shader_Vec4:   glUniform4fv(value_index, count, (float *)value); break;
        case Render_Shader_Int:    glUniform1iv(value_index, count, (int32_t *)value); break;
        case Render_Shader_Ivec2:  glUniform2iv(value_index, count, (int32_t *)value); break;
        case Render_Shader_Ivec3:  glUniform3iv(value_index, count, (int32_t *)value); break;
        case Render_Shader_Ivec4:  glUniform4iv(value_index, count, (int32_t *)value); break;
        case Render_Shader_Uint:   glUniform1uv(value_index, count, (uint32_t *)value); break;
        case Render_Shader_Uivec2: glUniform2uv(value_index, count, (uint32_t *)value); break;
        case Render_Shader_Uivec3: glUniform3uv(value_index, count, (uint32_t *)value); break;
        case Render_Shader_Uivec4: glUniform4uv(value_index, count, (uint32_t *)value); break;
    };
}
internal void render_shader_set_value(uint32_t value_index, const void *value, Render_Shader type)
{
    render_shader_set_value_vec(value_index, value, type, 1);
}
