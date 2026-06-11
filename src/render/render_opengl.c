// ak: Global variables
//=============================================================================

read_only global Str8 _render_opengl_shader_rect_vert_src = str8_comp("");
read_only global Str8 _render_opengl_shader_rect_frag_src = str8_comp("");

global read_only _Render_Opengl_Attribute _render_opengl_rect_input_attributes[] =
{
    {0, str8_comp(""), GL_FLOAT, 4},
};
global read_only _Render_Opengl_Attribute _render_opengl_single_color_output_attributes[] =
{
    {0, str8_comp(""), GL_INT, 4},
};

Str8 *_render_opengl_shader_kind_vert_src_table[1] = {
    &_render_opengl_shader_rect_vert_src,
};
Str8 *_render_opengl_shader_kind_frag_src_table[1] = {
    &_render_opengl_shader_rect_frag_src,
};

_Render_Opengl_Attribute_Array _render_opengl_shader_kind_input_attributes_table[_Render_Opengl_Shader_Kind_COUNT] =
{
    { _render_opengl_rect_input_attributes, ArrayLength(_render_opengl_rect_input_attributes) },
};

_Render_Opengl_Attribute_Array r_ogl_shader_kind_output_attributes_table[_Render_Opengl_Shader_Kind_COUNT] =
{
    { _render_opengl_single_color_output_attributes, ArrayLength(_render_opengl_single_color_output_attributes) },
};

// ak: OpenGL Helper functions
//=============================================================================

internal Render_Handle _render_handle_from_opengl_tex2d(_Render_Opengl_Tex2D *tex2d)
{
    Render_Handle handle = {(uintptr_t)tex2d};
    return handle;
}

internal _Render_Opengl_Tex2D *_render_opengl_tex2d_from_handle(Render_Handle handle)
{
  _Render_Opengl_Tex2D *tex2d = (_Render_Opengl_Tex2D *)handle.u64[0];
  return tex2d;
}

internal _Render_Opengl_FormatInfo _render_opengl_format_info_from_tex2d_format(Render_Tex2D_Format format)
{
    _Render_Opengl_FormatInfo result = ZERO_STRUCT;
    switch (format)
    {
        case Render_Tex2D_Format_R8:
        {
            result.internal_format = GL_R8;
            result.format = GL_RED;
            result.base_type = GL_UNSIGNED_BYTE;
        } break;
        case Render_Tex2D_Format_RG8:
        {
            result.internal_format = GL_RG8;
            result.format = GL_RG;
            result.base_type = GL_UNSIGNED_BYTE;
        } break;
        case Render_Tex2D_Format_RGBA8:
        {
            result.internal_format = GL_RGBA8;
            result.format = GL_RGBA;
            result.base_type = GL_UNSIGNED_BYTE;
        } break;
        case Render_Tex2D_Format_BGRA8:
        {
            result.internal_format = GL_RGBA8;
            result.format = GL_BGRA;
            result.base_type = GL_UNSIGNED_BYTE;
        } break;
        case Render_Tex2D_Format_R16:
        {
            result.internal_format = GL_R16;
            result.format = GL_RED;
            result.base_type = GL_UNSIGNED_SHORT;
        } break;
        case Render_Tex2D_Format_RGBA16:
        {
            result.internal_format = GL_RGBA16;
            result.format = GL_RGBA;
            result.base_type = GL_UNSIGNED_SHORT;
        } break;
        case Render_Tex2D_Format_R32:
        {
            result.internal_format = GL_R32F;
            result.format = GL_RED;
            result.base_type = GL_FLOAT;
        } break;
        case Render_Tex2D_Format_RG32:
        {
            result.internal_format = GL_RG32F;
            result.format = GL_RG;
            result.base_type = GL_FLOAT;
        } break;
        case Render_Tex2D_Format_RGBA32:
        {
            result.internal_format = GL_RGBA32F;
            result.format = GL_RGBA;
            result.base_type = GL_FLOAT;
        } break;
    }
    return result;
}

internal void _render_opengl_debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Unused(source);
    Unused(type);
    Unused(id);
    Unused(severity);
    Unused(userParam);
    fmt_eprintf("[OpenGL] %.*s\n", (int)length, message);
}

internal GLuint _render_opengl_instance_buffer_from_size(size_t size)
{
    GLuint buffer = _render_opengl_state->vbo;
    if(size > KB(64))
    {
        // rjf: build buffer
        size_t flushed_buffer_size = size;
        flushed_buffer_size += MB(1)-1;
        flushed_buffer_size -= flushed_buffer_size%MB(1);
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, flushed_buffer_size, 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // rjf: push buffer to flush list
        R_OGL_FlushBuffer *n = arena_push(_render_opengl_state->buffer_flush_arena, R_OGL_FlushBuffer, 1);
        n->id = buffer;
        SLLQueuePush(_render_opengl_state->first_buffer_to_flush, _render_opengl_state->last_buffer_to_flush, n);
    }
    return buffer;
}

internal bool _render_opengl_scissor(Rng2_F32 clip, Vec2_F32 viewport_dim)
{
    int32_t x0 = Clamp(0, (int32_t)floor_f32(clip.x0), (int32_t)viewport_dim.x);
    int32_t y0 = Clamp(0, (int32_t)floor_f32(clip.y0), (int32_t)viewport_dim.y);
    int32_t x1 = Clamp(0, (int32_t)ceil_f32(clip.x1), (int32_t)viewport_dim.x);
    int32_t y1 = Clamp(0, (int32_t)ceil_f32(clip.y1), (int32_t)viewport_dim.y);
    int32_t width = x1 - x0;
    int32_t height = y1 - y0;
    if(width > 0 && height > 0)
    {
        glScissor(x0, (int32_t)viewport_dim.y - y1, width, height);
    }
    return width > 0 && height > 0;
}

// Core functions
//=============================================================================

internal void render_init(Wl_Window window)
{
    // ak: do os-specific portion of work
    _render_opengl_init();
    
    // ak: top-level initialization
    if (wl_window_match(window, wl_window_zero())) {return;}
    Arena *arena = arena_alloc();
    _render_opengl_state = arena_push(arena, _Render_Opengl_State, 1);
    _render_opengl_state->arena = arena;
    _render_opengl_state->render_begin_arena_pos = arena_pos(arena);
    
    // ak: load opengl procedures
#define X(name, r, p) name = (name##_FunctionType *)(void*)_render_opengl_load_procedure(#name);
    RenderOpenglXMacro
#undef X
#ifdef BUILD_DEBUG
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(_render_opengl_debug_message_callback, 0);
#endif
    
    // ak: build all shaders
    for EachEnumVal(_Render_Opengl_Shader_Kind, k)
    {
        // ak: compile
        struct {
            GLenum type;
            Str8 *src;
            GLuint out;
            Str8 errors;
        } stages[] = {
            {
                GL_VERTEX_SHADER,
                _render_opengl_shader_kind_vert_src_table[k],
                0,
                str8_zero()
            },
            {
                GL_FRAGMENT_SHADER,
                _render_opengl_shader_kind_frag_src_table[k],
                0,
                str8_zero()
            },
        };
        for EachElement(idx, stages)
        {
            stages[idx].out = glCreateShader(stages[idx].type);
            GLint src_size = stages[idx].src->size;
#if OS_LINUX
            glShaderSource(stages[idx].out, 1, (const char * const*)&stages[idx].src->cstr, &src_size);
#else
            glShaderSource(stages[idx].out, 1, (char **)&stages[idx].src->cstr, &src_size);
#endif
            glCompileShader(stages[idx].out);
            GLint info_log_length = 0;
            GLint status = 0;
            glGetShaderiv(stages[idx].out, GL_COMPILE_STATUS, &status);
            glGetShaderiv(stages[idx].out, GL_INFO_LOG_LENGTH, &info_log_length);
            if(info_log_length != 0)
            {
                stages[idx].errors.cstr = arena_push(_render_opengl_state->arena, uint8_t, info_log_length+1);
                stages[idx].errors.size = info_log_length;
                glGetShaderInfoLog(stages[idx].out, info_log_length, 0, (char *)stages[idx].errors.cstr);
            }
            raddbg_pin(text(stages[idx].errors.cstr));
        }
        
        // ak: attach compilations to program
        GLuint program = glCreateProgram();
        for EachElement(idx, stages)
        {
            glAttachShader(program, stages[idx].out);
        }
        
        // ak: bind inputs
        _Render_Opengl_Attribute_Array inputs = _render_opengl_shader_kind_input_attributes_table[k];
        for EachIndex(idx, inputs.count)
        {
            glBindAttribLocation(program, inputs.v[idx].index, (char *)inputs.v[idx].name.cstr);
        }
        
        // ak: bind outputs
        _Render_Opengl_Attribute_Array outputs = _render_opengl_shader_kind_output_attributes_table[k];
        for EachIndex(idx, outputs.count)
        {
            glBindFragDataLocation(program, outputs.v[idx].index, (char *)outputs.v[idx].name.cstr);
        }
        
        // ak: link / validate / store
        {
            glLinkProgram(program);
            glValidateProgram(program);
            GLint status;
            glGetProgramiv(program, GL_LINK_STATUS, &status);
            if (!status)
            {
                char log[512];
                glGetProgramInfoLog(program, 512, NULL, log);
                LogErrorLine(&_os_core_state.log_context, "%s", log);
            }
            _render_opengl_state->shaders[k] = program;
        }
    }
    
    // ak: set up built-in resources
    {
        // ak: all-purpose VAO
        glGenVertexArrays(1, &_render_opengl_state->vao);
        
        // ak: scratch buffer
        {
            glGenBuffers(1, &_render_opengl_state->vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state->vbo);
            glBufferData(GL_ARRAY_BUFFER, KB(64), 0, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        // ak: white texture
        {
            glGenTextures(1, &_render_opengl_state->white_texture);
            glBindTexture(GL_TEXTURE_2D, _render_opengl_state->white_texture);
            uint32_t white_pixel = 0xffffffff;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white_pixel);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    
    // ak: set up options
    glEnable(GL_FRAMEBUFFER_SRGB);
}

internal Render_Handle render_window_equip(Wl_Window window)
{
    _Render_Opengl_Window *window_opengl = _render_opengl_state->free_window;
    if(window_opengl != 0)
    {
        SLLStackPop(_render_opengl_state->free_window);
    }
    else
    {
        window_opengl = arena_push_nz(_render_opengl_state->arena, _Render_Opengl_Window, 1);
    }
    MemSetZeroStruct(window_opengl);
    window_opengl->window_os = _render_opengl_window_equip(window);
    Render_Handle result = {(uint64_t)window_opengl};
    return result;
}

internal void render_window_unequip(Wl_Window window, Render_Handle window_equip)
{
    _Render_Opengl_Window *window_opengl = (_Render_Opengl_Window *)window_equip.u64[0];
    _render_opengl_window_unequip(window, window_opengl->window_os);
    SLLStackPush(_render_opengl_state->free_window, window_opengl);
}


internal void render_window_begin_frame(Wl_Window window, Render_Handle handle)
{
    _render_opengl_select_window(window, handle);
    _Render_Opengl_Window *window_opengl = (_Render_Opengl_Window *)handle.u64[0];
    
    // ak: unpack window viewport info
    Rng2_F32 canvas_rect = wl_canvas_rect_from_window(window);
    Vec2_F32 canvas_rect_dim = dim2(canvas_rect);
    
    // ak: set up targets if needed
    if(canvas_rect_dim.x != window_opengl->last_canvas_rect_dim.x || canvas_rect_dim.y != window_opengl->last_canvas_rect_dim.y)
    {
        window_opengl->last_canvas_rect_dim = canvas_rect_dim;
        _Render_Opengl_RenderTarget *targets[] =
        {
            &window_opengl->stage_target,
            &window_opengl->stage_scratch_target,
        };
        for EachElement(idx, targets)
        {
            if(targets[idx]->fbo != 0)
            {
                glDeleteFramebuffers(1, &targets[idx]->fbo);
                glDeleteTextures(1, &targets[idx]->color_texture);
            }
            glGenFramebuffers(1, &targets[idx]->fbo);
            glGenTextures(1, &targets[idx]->color_texture);
            glBindFramebufferScope(GL_FRAMEBUFFER, targets[idx]->fbo)
            glBindTextureScope(GL_TEXTURE_2D, targets[idx]->color_texture)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLint)canvas_rect_dim.x, (GLint)canvas_rect_dim.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targets[idx]->color_texture, 0);
            }
        }
    }
    
    // ak: clear and reset state
    {
        GLuint fbos[] =
        {
            window_opengl->stage_target.fbo,
            window_opengl->stage_scratch_target.fbo,
            0,
        };
        for EachElement(idx, fbos)
        {
            glBindFramebufferScope(GL_FRAMEBUFFER, fbos[idx])
            {
                glClearColor(0, 0, 0, 0);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }
        }
    }
    glViewport(0, 0, (GLsizei)canvas_rect_dim.x, (GLsizei)canvas_rect_dim.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

internal void render_window_end_frame(Wl_Window window, Render_Handle handle)
{
    _Render_Opengl_Window *window_opengl = (_Render_Opengl_Window *)handle.u64[0];
    
    // ak: blit window's main staging frame buffer to window frame buffer
    glBindFramebufferScope(GL_READ_FRAMEBUFFER, window_opengl->stage_target.fbo)
    glBindFramebufferScope(GL_DRAW_FRAMEBUFFER, 0)
    {
        glBlitFramebuffer(0, 0, (GLint)window_opengl->last_canvas_rect_dim.x, (GLint)window_opengl->last_canvas_rect_dim.y,
                0, 0, (GLint)window_opengl->last_canvas_rect_dim.x, (GLint)window_opengl->last_canvas_rect_dim.y,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST);
    }
    
    // ak: swap buffers
    _render_opengl_window_swap(window, handle);
}

typedef enum R_PassKind
{
    R_PassKind_UI,
    R_PassKind_COUNT,
} R_PassKind;

typedef struct R_Batch R_Batch;
struct R_Batch
{
    uint8_t *v;
    size_t byte_count;
    size_t byte_cap;
};

typedef struct R_BatchNode R_BatchNode;
struct R_BatchNode
{
    R_BatchNode *next;
    R_Batch v;
};

typedef struct R_BatchList R_BatchList;
struct R_BatchList
{
    R_BatchNode *first;
    R_BatchNode *last;
    size_t batch_count;
    size_t byte_count;
    size_t bytes_per_inst;
};

typedef enum R_Tex2DSampleKind
{
    R_Tex2DSampleKind_Nearest,
    R_Tex2DSampleKind_Linear,
    R_Tex2DSampleKind_COUNT,
} R_Tex2DSampleKind;

typedef struct _Render_Batch_Group_2D_Params _Render_Batch_Group_2D_Params;
struct _Render_Batch_Group_2D_Params
{
    Render_Handle tex;
    R_Tex2DSampleKind tex_sample_kind;
    Mat3x3_F32 xform;
    Rng2_F32 clip;
    float transparency;
};

typedef struct _Render_Batch_Group_2D_Node _Render_Batch_Group_2D_Node;
struct _Render_Batch_Group_2D_Node
{
    _Render_Batch_Group_2D_Node *next;
    R_BatchList batches;
    _Render_Batch_Group_2D_Params params;
};

typedef struct _Render_Batch_Group_2D_List _Render_Batch_Group_2D_List;
struct _Render_Batch_Group_2D_List
{
    _Render_Batch_Group_2D_Node *first;
    _Render_Batch_Group_2D_Node *last;
    size_t count;
};

typedef struct R_PassParams_UI R_PassParams_UI;
struct R_PassParams_UI
{
    _Render_Batch_Group_2D_List rects;
};

typedef struct R_Pass R_Pass;
struct R_Pass
{
    R_PassKind kind;
    union
    {
        void *params;
        R_PassParams_UI *params_ui;
    };
};

typedef struct R_PassNode R_PassNode;
struct R_PassNode
{
    R_PassNode *next;
    R_Pass v;
};

typedef struct R_PassList R_PassList;
struct R_PassList
{
    R_PassNode *first;
    R_PassNode *last;
    size_t count;
};

internal void render_window_submit(Wl_Window window, Render_Handle window_equip, R_PassList *passes)
{
    _Render_Opengl_Window *w = (_Render_Opengl_Window *)window_equip.u64[0];
    Rng2_F32 viewport_rect = wl_canvas_rect_from_window(window);
    Vec2_F32 viewport_dim = dim2(viewport_rect);
    for(R_PassNode *pass_n = passes->first; pass_n != 0; pass_n = pass_n->next)
    {
        R_Pass *pass = &pass_n->v;
        switch(pass->kind)
        {
            // ak: ui rendering pass
            case R_PassKind_UI:
            {
                //- rjf: unpack params
                R_PassParams_UI *params = pass->params_ui;
                _Render_Batch_Group_2D_List *rect_batch_groups = &params->rects;
                
                //- rjf: draw each batch group
                GLuint shader = _render_opengl_state->shaders[_Render_Opengl_Shader_Kind_Rect];
                glUseProgramScope(shader)
                glBindVertexArrayScope(_render_opengl_state->vao)
                glBindFramebufferScope(GL_FRAMEBUFFER, w->stage_target.fbo)
                {
                    for(_Render_Batch_Group_2D_Node *group_n = rect_batch_groups->first; group_n != 0; group_n = group_n->next)
                    {
                        R_BatchList *batches = &group_n->batches;
                        _Render_Batch_Group_2D_Params *group_params = &group_n->params;
                        
                        //- rjf: unpack texture
                        Render_Tex2D_Format texture_fmt = Render_Tex2D_Format_RGBA8;
                        GLuint texture_id = _render_opengl_state->white_texture;
                        {
                            _Render_Opengl_Tex2D *tex = _render_opengl_tex2d_from_handle(group_params->tex);
                            if(tex != 0)
                            {
                                texture_id = tex->id;
                                texture_fmt = tex->format;
                            }
                        }
                        
                        //- rjf: get & fill buffer
                        GLuint buffer = _render_opengl_instance_buffer_from_size(batches->byte_count);
                        {
                            glBindBuffer(GL_ARRAY_BUFFER, buffer);
                            size_t off = 0;
                            for(R_BatchNode *batch_n = batches->first; batch_n != 0; batch_n = batch_n->next)
                            {
                                glBufferSubData(GL_ARRAY_BUFFER, off, batch_n->v.byte_count, batch_n->v.v);
                                off += batch_n->v.byte_count;
                            }
                        }
                        
                        //- rjf: bind input attributes
                        {
                            _Render_Opengl_Attribute_Array inputs = _render_opengl_shader_kind_input_attributes_table[_Render_Opengl_Shader_Kind_Rect];
                            size_t off = 0;
                            for EachIndex(idx, inputs.count)
                            {
                                glEnableVertexAttribArray(inputs.v[idx].index);
                                glVertexAttribDivisor(inputs.v[idx].index, 1);
                                glVertexAttribPointer(inputs.v[idx].index, inputs.v[idx].count, inputs.v[idx].type, GL_FALSE, sizeof(R_Rect2DInst), (void *)(off));
                                // TODO(rjf): this is not correct if type != GL_FLOAT
                                off += inputs.v[idx].count*sizeof(GL_FLOAT);
                            }
                        }
                        
                        //- rjf: bind texture
                        {
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, texture_id);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                            switch(group_params->tex_sample_kind)
                            {
                                default:
                                case R_Tex2DSampleKind_Nearest:
                                {
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                                }break;
                                case R_Tex2DSampleKind_Linear:
                                {
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                                }break;
                            }
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
                            glUniform1i(glGetUniformLocation(shader, "u_tex_color"), 0);
                        }
                        
                        //- rjf: upload misc. uniforms
                        {
                            Mat4x4_F32 texture_sample_channel_map = render_sample_channel_map_from_tex2dformat(texture_fmt);
                            glUniformMatrix4fv(glGetUniformLocation(shader, "u_texture_sample_channel_map"), 1, 0, &texture_sample_channel_map.v[0][0]);
                            glUniform2f(glGetUniformLocation(shader, "u_viewport_size_px"), viewport_dim.x, viewport_dim.y);
                            glUniform1f(glGetUniformLocation(shader, "u_opacity"), 1.f - group_params->transparency);
                            glUniformMatrix3fv(glGetUniformLocation(shader, "u_xform"), 1, 0, &group_params->xform.v[0][0]);
                        }
                        
                        //- rjf: set up scissor
                        if(group_params->clip.x0 != 0 ||
                            group_params->clip.x1 != 0 ||
                            group_params->clip.y0 != 0 ||
                            group_params->clip.y1 != 0)
                        {
                            if(_render_opengl_scissor(group_params->clip, viewport_dim))
                            {
                                glEnable(GL_SCISSOR_TEST);
                            }
                        }
                        
                        //- rjf: draw
                        {
                            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batches->byte_count / batches->bytes_per_inst);
                        }
                        
                        //- rjf: unset scissor
                        glDisable(GL_SCISSOR_TEST);
                    }
                }
            } break;
            case R_PassKind_COUNT:
            {
            } break;
        }
    }
}

internal void render_end(void)
{
    uint32_t win_width = wl_window_width_get(_render_opengl_state->window);
    uint32_t win_height = wl_window_height_get(_render_opengl_state->window);
    glViewport(0, 0, win_width, win_height);
    glClearColor(1.f, 0.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    // start shader program
    glUseProgram(_render_opengl_state.shader);
    glBindVertexArray(_render_opengl_state.vao);
    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_resolution"), (GLfloat)win_width, (GLfloat)win_height);
    for (Render_Draw_Node *node = _render_opengl_state->list.first; node != NULL; node = node->next)
    {
        switch (node->type)
        {
            case Render_Draw_Type_Rect:
            {
                glBindBuffer(GL_ARRAY_BUFFER, _render_opengl_state.vbo);
                Render_Draw_Rect rect = node->param_rect;
                float x = rect.dst.x;
                float y = rect.dst.y;
                float width = rect.dst.z;
                float height = rect.dst.w;
                Vec4_F32 color = rect.color;
                // ak: bind texture
                {
                    GLuint texture_id = _render_opengl_state.default_texture;
                    _Render_Opengl_Tex2D *tex2d = _render_opengl_tex2d_from_handle(rect.texture);
                    if (tex2d != 0)
                    {
                        texture_id = tex2d->id;
                    }
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                }
                // ak: fill shader location
                {
                    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_offset"), x, y);
                    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_size"),   width, height);
                    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_uv_offset"), rect.src.x, rect.src.y);
                    glUniform2f(glGetUniformLocation(_render_opengl_state.shader, "u_uv_size"),   rect.src.z, rect.src.w);
                    glUniform4f(glGetUniformLocation(_render_opengl_state.shader, "u_color"),  color.x, color.y, color.z, color.w);
                }
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindTexture(GL_TEXTURE_2D, 0);
            } break;
        }
    }
    glBindVertexArray(0);
    glUseProgram(0);
    
    // ak: os internal opengl
    _render_opengl();
}

// ak: Texture functions
//=============================================================================

internal Render_Handle render_tex2d_alloc(Render_Resource_Kind kind, Render_Tex2D_Format format, Vec2_I32 size, void *data, Arena *arena)
{
    // ak: allocate texture record
    _Render_Opengl_Tex2D *tex2d = _render_opengl_state->free_tex2d;
    if(tex2d)
    {
        SLLStackPop(_render_opengl_state->free_tex2d);
    }
    else
    {
        tex2d = arena_push(arena, _Render_Opengl_Tex2D, 1);
    }
    
    // ak: map kind/format -> gl counterparts
    _Render_Opengl_FormatInfo format_info = _render_opengl_format_info_from_tex2d_format(format);
    
    // ak: allocate GL texture
    glGenTextures(1, &tex2d->id);
    glBindTextureScope(GL_TEXTURE_2D, tex2d->id)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format_info.internal_format, size.x, size.y, 0, format_info.format, format_info.base_type, data);
    }
    
    // ak: fill texture data
    tex2d->resource_kind = kind;
    tex2d->format = format;
    tex2d->size = size;
    
    // ak: bundle & return
    Render_Handle result = _render_handle_from_opengl_tex2d(tex2d);
    return result;
}

internal void render_tex2d_free(Render_Handle handle)
{
    _Render_Opengl_Tex2D *tex2d = _render_opengl_tex2d_from_handle(handle);
    if (tex2d != 0)
    {
        glDeleteTextures(1, &tex2d->id);
        SLLStackPush(_render_opengl_state->free_tex2d, tex2d);
    }
}

