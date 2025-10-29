// Helper functions
//=============================================================================

internal U32 render_opengl_shader_compile(Str8 source, GLenum type, U32 program_id)
{
    GLuint shader_id = glCreateShader(type);
    const char *shader_source = (const char *)source.cstr;
    glShaderSource(shader_id, 1, &shader_source, NULL);
    glCompileShader(shader_id);
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char log[512];
        glGetShaderInfoLog(shader_id, 512, NULL, log);
        fmt_printf("Shader program linking failed: %s", log);
    }
    glAttachShader(program_id, shader_id);
    return shader_id;
}

// Core functions
//=============================================================================

internal void render_init(void)
{
    render_opengl_init();

#define X(name, r, p) name = (name##_FunctionType *)(void*)render_opengl_load_procedure(#name);
    RenderOpenglXMacro
#undef X

    glGenVertexArrays(1, &render_opengl_state.vertex_arrays);
    glBindVertexArray(render_opengl_state.vertex_arrays);

    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f
    };
    render_opengl_state.array_buffer_size = sizeof(vertices);
    glGenBuffers(1, &render_opengl_state.array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, render_opengl_state.array_buffer);
    glBufferData(GL_ARRAY_BUFFER, render_opengl_state.array_buffer_size, vertices, GL_STATIC_DRAW);
}

internal void render_deinit(void)
{
    render_opengl_deinit();
    glDeleteVertexArrays(sizeof(&render_opengl_state.vertex_arrays), &render_opengl_state.vertex_arrays);
    glDeleteBuffers(render_opengl_state.array_buffer_size, &render_opengl_state.array_buffer);
}

internal void render_begin(void)
{
    render_opengl_begin();
}

internal void render_end(void)
{
    render_opengl_end();
}

internal U32 render_shader_load(Str8 vert_source, Str8 frag_source)
{
    U32 program_id = glCreateProgram();
    U32 vert_id = render_opengl_shader_compile(vert_source, GL_VERTEX_SHADER, program_id);
    U32 frag_id = render_opengl_shader_compile(frag_source, GL_FRAGMENT_SHADER, program_id);
    glLinkProgram(program_id);
    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if (!status)
    {
        char log[512];
        glGetProgramInfoLog(program_id, 512, NULL, log);
        fmt_printf("Shader program linking failed: %s", log);
    }
    glUseProgram(program_id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    return program_id;
}

internal void render_shader_unload(U32 id)
{
    glDeleteProgram(id);
}

internal U32 render_shader_get_value(U32 shader_id, Str8 name)
{
    return glGetUniformLocation(shader_id, (char *)name.cstr);
}

internal void render_shader_set_value_vec(
    U32 value_index, const void *value, Render_Shader type, I32 count
) {
    switch (type)
    {
        case Render_Shader_Float:  glUniform1fv(value_index, count, (F32 *)value); break;
        case Render_Shader_Vec2:   glUniform2fv(value_index, count, (F32 *)value); break;
        case Render_Shader_Vec3:   glUniform3fv(value_index, count, (F32 *)value); break;
        case Render_Shader_Vec4:   glUniform4fv(value_index, count, (F32 *)value); break;
        case Render_Shader_Int:    glUniform1iv(value_index, count, (I32 *)value); break;
        case Render_Shader_Ivec2:  glUniform2iv(value_index, count, (I32 *)value); break;
        case Render_Shader_Ivec3:  glUniform3iv(value_index, count, (I32 *)value); break;
        case Render_Shader_Ivec4:  glUniform4iv(value_index, count, (I32 *)value); break;
        case Render_Shader_Uint:   glUniform1uv(value_index, count, (U32 *)value); break;
        case Render_Shader_Uivec2: glUniform2uv(value_index, count, (U32 *)value); break;
        case Render_Shader_Uivec3: glUniform3uv(value_index, count, (U32 *)value); break;
        case Render_Shader_Uivec4: glUniform4uv(value_index, count, (U32 *)value); break;
    };
}
internal void render_shader_set_value(
    U32 value_index, const void *value, Render_Shader type
) {
    render_shader_set_value_vec(value_index, value, type, 1);
}
