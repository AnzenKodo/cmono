internal U32 render_shader_compile(Str8 source, GLenum type, U32 program_id)
{
    GLuint shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, (char **)&source.cstr, NULL);
    glCompileShader(shader_id);
    GLint status;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
    if (!status) 
    {
        char log[512];
        glGetShaderInfoLog(shader_id, 512, NULL, log);
        printf("Shader program linking failed: %s", log);
    }
    glAttachShader(program_id, shader_id);
    return shader_id;
}

internal U32 render_shader_load(Str8 vert_source, Str8 frag_source)
{
    U32 program_id = glCreateProgram();
    U32 vert_id = render_shader_compile(vert_source, GL_VERTEX_SHADER, program_id);
    U32 frag_id = render_shader_compile(frag_source, GL_FRAGMENT_SHADER, program_id);
    glLinkProgram(program_id);
    GLint status;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if (!status) 
    {
        char log[512];
        glGetProgramInfoLog(program_id, 512, NULL, log);
        printf("Shader program linking failed: %s", log);
    }
    glUseProgram(program_id);
    // glDeleteShader(vert_id);
    // glDeleteShader(frag_id);
    return program_id;
}

internal void render_shader_unload(U32 id)
{
    glDeleteProgram(id);
}
