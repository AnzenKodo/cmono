#define SOKOL_GFX_IMPL
#define SOKOL_LOG_IMPL
#define SOKOL_GLCORE
#undef internal
#include "vendor/sokol_gfx.h"
#define internal static

// ak: GLSL 3.30 shader sources
//=============================================================================

global const char *_render_sokol_rect_vs_src =
    "#version 330 core\n"
    "layout(location = 0) in vec4 c2v_dst_rect;\n"
    "layout(location = 1) in vec4 c2v_src_rect;\n"
    "layout(location = 2) in vec4 c2v_colors_0;\n"
    "layout(location = 3) in vec4 c2v_colors_1;\n"
    "layout(location = 4) in vec4 c2v_colors_2;\n"
    "layout(location = 5) in vec4 c2v_colors_3;\n"
    "layout(location = 6) in vec4 c2v_corner_radii;\n"
    "layout(location = 7) in vec4 c2v_style;\n" // x:border_thickness y:softness z:omit_texture w:shear
    "\n"
    "out vec2  v2p_sdf_sample_pos;\n"
    "out vec2  v2p_texcoord_pct;\n"
    "out vec2  v2p_rect_half_size_px;\n"
    "out vec4  v2p_tint;\n"
    "out float v2p_corner_radius;\n"
    "out float v2p_border_thickness;\n"
    "out float v2p_softness;\n"
    "out float v2p_omit_texture;\n"
    "\n"
    "uniform vec4 u_col0;\n"
    "uniform vec4 u_col1;\n"
    "uniform vec4 u_col2;\n"
    "uniform vec4 u_vp_px;\n"
    "uniform sampler2D u_tex;\n"
    "\n"
    "void main(void) {\n"
    "  int vid = gl_VertexID & 3;\n"
    "  mat3 u_xform = mat3(u_col0.xyz, u_col1.xyz, u_col2.xyz);\n"
    "  vec2 u_viewport_size_px = u_vp_px.xy;\n"
    "  vec2 vertices[4];\n"
    "  vertices[0] = vec2(-1.0, -1.0);\n"
    "  vertices[1] = vec2(-1.0, +1.0);\n"
    "  vertices[2] = vec2(+1.0, -1.0);\n"
    "  vertices[3] = vec2(+1.0, +1.0);\n"
    "  float shears[4];\n"
    "  shears[0] = 0.0; shears[1] = 0.0;\n"
    "  shears[2] = c2v_style.w; shears[3] = c2v_style.w;\n"
    "  vec2 dst_half = (c2v_dst_rect.zw - c2v_dst_rect.xy) * 0.5;\n"
    "  vec2 dst_ctr  = (c2v_dst_rect.zw + c2v_dst_rect.xy) * 0.5;\n"
    "  vec2 dst_pos  = vertices[vid] * dst_half + dst_ctr;\n"
    "  dst_pos.y += shears[vid];\n"
    "  dst_pos = (u_xform * vec3(dst_pos, 1.0)).xy;\n"
    "  vec2 src_half = (c2v_src_rect.zw - c2v_src_rect.xy) * 0.5;\n"
    "  vec2 src_ctr  = (c2v_src_rect.zw + c2v_src_rect.xy) * 0.5;\n"
    "  vec2 src_pos  = vertices[vid] * src_half + src_ctr;\n"
    "  vec4 colors[4];\n"
    "  colors[0] = c2v_colors_0; colors[1] = c2v_colors_1;\n"
    "  colors[2] = c2v_colors_2; colors[3] = c2v_colors_3;\n"
    "  float cr[4];\n"
    "  cr[0] = c2v_corner_radii.x; cr[1] = c2v_corner_radii.y;\n"
    "  cr[2] = c2v_corner_radii.z; cr[3] = c2v_corner_radii.w;\n"
    "  ivec2 tsz_i = textureSize(u_tex, 0);\n"
    "  vec2  tsz   = vec2(float(tsz_i.x), float(tsz_i.y));\n"
    "  vec2 dst_verts_pct = vec2(((vid >> 1) != 1) ? 1.0 : 0.0,\n"
    "                            ((vid & 1) != 0) ? 0.0 : 1.0);\n"
    "  gl_Position = vec4(\n"
    "     2.0 * dst_pos.x / u_viewport_size_px.x - 1.0,\n"
    "     2.0 * (1.0 - dst_pos.y / u_viewport_size_px.y) - 1.0,\n"
    "     0.0, 1.0);\n"
    "  v2p_sdf_sample_pos    = (2.0 * dst_verts_pct - 1.0) * dst_half;\n"
    "  v2p_texcoord_pct      = src_pos / tsz;\n"
    "  v2p_rect_half_size_px = dst_half;\n"
    "  v2p_tint              = colors[vid];\n"
    "  v2p_corner_radius     = cr[vid];\n"
    "  v2p_border_thickness  = c2v_style.x;\n"
    "  v2p_softness          = c2v_style.y;\n"
    "  v2p_omit_texture      = c2v_style.z;\n"
    "}\n";

global const char *_render_sokol_rect_fs_src =
    "#version 330 core\n"
    "\n"
    "in vec2  v2p_sdf_sample_pos;\n"
    "in vec2  v2p_texcoord_pct;\n"
    "in vec2  v2p_rect_half_size_px;\n"
    "in vec4  v2p_tint;\n"
    "in float v2p_corner_radius;\n"
    "in float v2p_border_thickness;\n"
    "in float v2p_softness;\n"
    "in float v2p_omit_texture;\n"
    "\n"
    "out vec4 final_color;\n"
    "\n"
    "uniform sampler2D u_tex;\n"
    "uniform mat4  u_channel_map;\n"
    "uniform vec4  u_fs_params;\n" // u_fs_params.x = opacity
    "\n"
    "float rect_sdf(vec2 p, vec2 hs, float r) {\n"
    "  return length(max(abs(p) - hs + r, 0.0)) - r;\n"
    "}\n"
    "float srgb_to_linear(float x) {\n"
    "  return x < 0.0404482362771082 ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);\n"
    "}\n"
    "vec4 srgba_to_linear(vec4 v) {\n"
    "  return vec4(srgb_to_linear(v.r), srgb_to_linear(v.g),\n"
    "              srgb_to_linear(v.b), v.a);\n"
    "}\n"
    "void main(void) {\n"
    "  vec4 albedo = vec4(1.0);\n"
    "  if (v2p_omit_texture < 1.0) {\n"
    "    albedo = u_channel_map * texture(u_tex, v2p_texcoord_pct);\n"
    "    albedo = srgba_to_linear(albedo);\n"
    "  }\n"
    "  float border_t = 1.0;\n"
    "  if (v2p_border_thickness > 0.0) {\n"
    "    float s = rect_sdf(v2p_sdf_sample_pos,\n"
    "      v2p_rect_half_size_px - vec2(v2p_softness * 2.0) - v2p_border_thickness,\n"
    "      max(v2p_corner_radius - v2p_border_thickness, 0.0));\n"
    "    border_t = smoothstep(0.0, 2.0 * v2p_softness, s);\n"
    "  }\n"
    "  if (border_t < 0.001) discard;\n"
    "  float corner_t = 1.0;\n"
    "  if (v2p_corner_radius > 0.0 || v2p_softness > 0.75) {\n"
    "    float s = rect_sdf(v2p_sdf_sample_pos,\n"
    "      v2p_rect_half_size_px - vec2(v2p_softness * 2.0),\n"
    "      v2p_corner_radius);\n"
    "    corner_t = 1.0 - smoothstep(0.0, 2.0 * v2p_softness, s);\n"
    "  }\n"
    "  final_color   = albedo * v2p_tint;\n"
    "  final_color.a *= u_fs_params.x * corner_t * border_t;\n"
    "}\n";

// ak: Private Function Definition
//=============================================================================

//- ak: Helper ================================================================

internal sg_pixel_format _render_sokol_pixel_format(Render_Tex_2D_Format format);
internal size_t _render_sokol_bytes_per_pixel(Render_Tex_2D_Format format);
internal sg_view _render_sokol_texture_view(sg_image image);
internal void _render_slog_func(const char* tag, uint32_t log_level, uint32_t log_item, const char* message, uint32_t line_nr, const char* filename, void* user_data);

// ak: Function Implementation
//=============================================================================

//- ak: Helper ================================================================

internal sg_pixel_format _render_sokol_pixel_format(Render_Tex_2D_Format format)
{
    switch(format)
    {
        default:
        case Render_Tex_2D_Format_RGBA8:  return SG_PIXELFORMAT_RGBA8;
        case Render_Tex_2D_Format_R8:     return SG_PIXELFORMAT_R8;
        case Render_Tex_2D_Format_RG8:    return SG_PIXELFORMAT_RG8;
        case Render_Tex_2D_Format_BGRA8:  return SG_PIXELFORMAT_BGRA8;
        case Render_Tex_2D_Format_R16:    return SG_PIXELFORMAT_R16;
        case Render_Tex_2D_Format_RGBA16: return SG_PIXELFORMAT_RGBA16;
        case Render_Tex_2D_Format_R32:    return SG_PIXELFORMAT_R32F;
        case Render_Tex_2D_Format_RG32:   return SG_PIXELFORMAT_RG32F;
        case Render_Tex_2D_Format_RGBA32: return SG_PIXELFORMAT_RGBA32F;
    }
}

internal size_t _render_sokol_bytes_per_pixel(Render_Tex_2D_Format format)
{
    switch(format)
    {
        case Render_Tex_2D_Format_R8:     return 1;
        case Render_Tex_2D_Format_RG8:    return 2;
        case Render_Tex_2D_Format_RGBA8:  return 4;
        case Render_Tex_2D_Format_BGRA8:  return 4;
        case Render_Tex_2D_Format_R16:    return 2;
        case Render_Tex_2D_Format_RGBA16: return 8;
        case Render_Tex_2D_Format_R32:    return 4;
        case Render_Tex_2D_Format_RG32:   return 8;
        case Render_Tex_2D_Format_RGBA32: return 16;
        default:                          return 4;
    }
}

internal sg_view _render_sokol_texture_view(sg_image image)
{
    return sg_make_view(&(sg_view_desc){
        .texture = {
            .image = image,
        },
    });
}

internal void _render_slog_func(const char* tag, uint32_t log_level, uint32_t log_item, const char* message, uint32_t line_nr, const char* filename, void* user_data)
{
    Unused(log_item); Unused(user_data);
    Log_Level level = Log_Level_Info;
    switch (log_level) {
        case 0: level = Log_Level_Error; break;
        case 1: level = Log_Level_Error; break;
        case 2: level = Log_Level_Warn;  break;
    }
    log_tracef(log_trace_g, level, filename, line_nr, "%s: %s", tag, message);
}

//- ak: Pipeline Creation =====================================================

internal sg_pipeline _render_sokol_make_rect_pipeline(void)
{
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .label = "rect_shd",
        .vertex_func   = { .source = _render_sokol_rect_vs_src, .entry = "main" },
        .fragment_func = { .source = _render_sokol_rect_fs_src, .entry = "main" },
        .attrs = {
            [0] = { .glsl_name = "c2v_dst_rect" },
            [1] = { .glsl_name = "c2v_src_rect" },
            [2] = { .glsl_name = "c2v_colors_0" },
            [3] = { .glsl_name = "c2v_colors_1" },
            [4] = { .glsl_name = "c2v_colors_2" },
            [5] = { .glsl_name = "c2v_colors_3" },
            [6] = { .glsl_name = "c2v_corner_radii" },
            [7] = { .glsl_name = "c2v_style" },
        },
        .uniform_blocks[0] = {
            .stage  = SG_SHADERSTAGE_VERTEX,
            .size   = sizeof(float) * 16, // 4 * vec4
            .layout = SG_UNIFORMLAYOUT_NATIVE,
            .glsl_uniforms[0] = { .type = SG_UNIFORMTYPE_FLOAT4, .glsl_name = "u_col0",  .array_count = 1 },
            .glsl_uniforms[1] = { .type = SG_UNIFORMTYPE_FLOAT4, .glsl_name = "u_col1",  .array_count = 1 },
            .glsl_uniforms[2] = { .type = SG_UNIFORMTYPE_FLOAT4, .glsl_name = "u_col2",  .array_count = 1 },
            .glsl_uniforms[3] = { .type = SG_UNIFORMTYPE_FLOAT4, .glsl_name = "u_vp_px", .array_count = 1 },
        },
        .uniform_blocks[1] = {
            .stage  = SG_SHADERSTAGE_FRAGMENT,
            .size   = sizeof(float) * 20, // 16f mat4 + 4f (opacity + 3 pad)
            .layout = SG_UNIFORMLAYOUT_NATIVE,
            .glsl_uniforms[0] = { .type = SG_UNIFORMTYPE_MAT4,   .glsl_name = "u_channel_map", .array_count = 1 },
            .glsl_uniforms[1] = { .type = SG_UNIFORMTYPE_FLOAT4, .glsl_name = "u_fs_params",   .array_count = 1 },
        },
        .views[0] = {
            .texture = {
                .stage       = SG_SHADERSTAGE_FRAGMENT,
                .image_type  = SG_IMAGETYPE_2D,
                .sample_type = SG_IMAGESAMPLETYPE_FLOAT,
            },
        },
        .samplers[0] = {
            .stage        = SG_SHADERSTAGE_FRAGMENT,
            .sampler_type = SG_SAMPLERTYPE_FILTERING,
        },
        .texture_sampler_pairs[0] = {
            .stage        = SG_SHADERSTAGE_FRAGMENT,
            .view_slot    = 0,
            .sampler_slot = 0,
            .glsl_name    = "u_tex",
        },
    });
    
    sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
        .label  = "rect_pip",
        .shader = shd,
        .layout = {
            .buffers[0] = {
                .stride    = sizeof(Render_Rect_2D_Inst),
                .step_func = SG_VERTEXSTEP_PER_INSTANCE,
            },
            .attrs = {
                [0] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, dst),                    .format = SG_VERTEXFORMAT_FLOAT4 },
                [1] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, src),                    .format = SG_VERTEXFORMAT_FLOAT4 },
                [2] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, colors[0]),             .format = SG_VERTEXFORMAT_FLOAT4 },
                [3] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, colors[1]),             .format = SG_VERTEXFORMAT_FLOAT4 },
                [4] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, colors[2]),             .format = SG_VERTEXFORMAT_FLOAT4 },
                [5] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, colors[3]),             .format = SG_VERTEXFORMAT_FLOAT4 },
                [6] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, corner_radii),          .format = SG_VERTEXFORMAT_FLOAT4 },
                [7] = { .buffer_index = 0, .offset = OffsetOf(Render_Rect_2D_Inst, border_thickness),      .format = SG_VERTEXFORMAT_FLOAT4 },
            },
        },
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
        .colors[0] = {
            .pixel_format = SG_PIXELFORMAT_SRGB8A8,
            .blend = {
                .enabled          = true,
                .src_factor_rgb   = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb   = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
            },
        },
        .depth = {
            .pixel_format = SG_PIXELFORMAT_DEPTH_STENCIL,
            .compare      = SG_COMPAREFUNC_ALWAYS,
            .write_enabled = false,
        },
    });
    
    _render_sokol_state->shd_rect = shd;
    return pip;
}

//- ak: Backend Hooks Implementation ==========================================

//- ak: Layer Init & Cleanup ==================================================

internal void render_init(void)
{
    Arena *arena = arena_alloc();
    _render_sokol_state = arena_push(arena, _Render_Sokol_State, 1);
    _render_sokol_state->arena = arena;
}

internal void _render_sokol_ensure_initialized(void)
{
    if(!_render_sokol_state || _render_sokol_state->initialized) return;
    
    sg_setup(&(sg_desc){
        .logger = { .func = _render_slog_func },
    });
    
    // ak: white 1x1 RGBA8 fallback texture
    {
        uint8_t w[4] = { 255, 255, 255, 255 };
        _render_sokol_state->white_image = sg_make_image(&(sg_image_desc){
            .type         = SG_IMAGETYPE_2D,
            .width        = 1,
            .height       = 1,
            .num_slices   = 1,
            .num_mipmaps  = 1,
            .pixel_format = SG_PIXELFORMAT_RGBA8,
            .data         = (sg_range){ .ptr = w, .size = sizeof(w) },
            .label        = "white_1x1",
        });
        _render_sokol_state->white_sampler = sg_make_sampler(&(sg_sampler_desc){
            .min_filter = SG_FILTER_NEAREST,
            .mag_filter = SG_FILTER_NEAREST,
            .wrap_u     = SG_WRAP_REPEAT,
            .wrap_v     = SG_WRAP_REPEAT,
            .label      = "white_smp",
        });
        _render_sokol_state->white_view = _render_sokol_texture_view(_render_sokol_state->white_image);
    }
    
    // ak: Pipeline
    _render_sokol_state->pip_rect = _render_sokol_make_rect_pipeline();
    
    // ak: Streaming / transient instance buffer (1 MB initial)
    {
        size_t sz = MB(1);
        _render_sokol_state->inst_buf_size = sz;
        _render_sokol_state->inst_buf = sg_make_buffer(&(sg_buffer_desc){
            .size  = sz,
            .usage = {
                .vertex_buffer   = true,
                .write_transient = true,
            },
            .label = "rect_inst",
        });
    }
    
    _render_sokol_state->initialized = true;
}

internal void render_cleanup(void)
{
    if(!_render_sokol_state) return;
    
    if(_render_sokol_state->initialized)
    {
        sg_destroy_view(_render_sokol_state->white_view);
        sg_destroy_image(_render_sokol_state->white_image);
        sg_destroy_sampler(_render_sokol_state->white_sampler);
        sg_destroy_pipeline(_render_sokol_state->pip_rect);
        sg_destroy_shader(_render_sokol_state->shd_rect);
        sg_destroy_buffer(_render_sokol_state->inst_buf);
        if(_render_sokol_state->staging_buf.id != SG_INVALID_ID)
        {
            sg_destroy_buffer(_render_sokol_state->staging_buf);
        }
        
        for(_Render_Sokol_Tex_2D *t = _render_sokol_state->free_tex2d; t != NULL; t = t->next)
        {
            if(t->view_nearest.id != SG_INVALID_ID) sg_destroy_view(t->view_nearest);
            if(t->view_linear.id  != SG_INVALID_ID) sg_destroy_view(t->view_linear);
            if(t->image.id        != SG_INVALID_ID) sg_destroy_image(t->image);
            if(t->smp_nearest.id  != SG_INVALID_ID) sg_destroy_sampler(t->smp_nearest);
            if(t->smp_linear.id   != SG_INVALID_ID) sg_destroy_sampler(t->smp_linear);
        }
        
        sg_shutdown();
    }
    
    arena_free(_render_sokol_state->arena);
    _render_sokol_state = NULL;
}

//- ak: Window Setup / Teardown ===============================================

internal Render_Handle render_window_equip(Wl_Window window)
{
    wl_window_make_current(window);
    _render_sokol_ensure_initialized();
    
    _Render_Sokol_Window *sw = _render_sokol_state->free_window;
    if(sw != NULL)
    {
        SLLStackPop(_render_sokol_state->free_window);
    }
    else
    {
        sw = arena_push(_render_sokol_state->arena, _Render_Sokol_Window, 1);
    }
    MemSetZeroStruct(sw);
    sw->window = window;
    Render_Handle result = HandlePack(Render_Handle, sw);
    return result;
}

internal void render_window_unequip(Wl_Window window, Render_Handle window_equip)
{
    Unused(window);
    _Render_Sokol_Window *sw = HandleUnpack(_Render_Sokol_Window, window_equip);
    if(sw != NULL)
    {
        SLLStackPush(_render_sokol_state->free_window, sw);
    }
}

//- ak: Frame Markers =========================================================

internal void render_begin_frame(void)
{
}

internal void render_end_frame(void)
{
    sg_commit();
}

internal void render_window_begin_frame(Wl_Window window, Render_Handle handle)
{
    Unused(handle);
    wl_window_make_current(window);
}

internal void render_window_end_frame(Wl_Window window, Render_Handle handle)
{
    Unused(handle);
    wl_window_swap_buffers(window);
}

//- ak: Render Pass Submission ================================================

internal void render_window_submit(Wl_Window window, Render_Handle window_equip, Render_Pass_List *passes)
{
    _Render_Sokol_Window *sw = HandleUnpack(_Render_Sokol_Window, window_equip);
    if(!sw) return;
    
    Rng2_F32 canvas = wl_canvas_rect_from_window(window);
    Vec2_F32 vp_dim = dim_rng2(canvas);
    int vp_w = (int)vp_dim.x;
    int vp_h = (int)vp_dim.y;
    if(vp_w <= 0 || vp_h <= 0) return;
    
    sg_swapchain sc = {
        .width        = vp_w,
        .height       = vp_h,
        .sample_count = 1,
        .color_format = SG_PIXELFORMAT_SRGB8A8,
        .depth_format = SG_PIXELFORMAT_DEPTH_STENCIL,
        .gl           = { .framebuffer = 0 },
    };
    
    sg_begin_pass(&(sg_pass){
        .action = {
            .colors[0] = {
                .load_action  = SG_LOADACTION_CLEAR,
                .store_action = SG_STOREACTION_STORE,
                .clear_value  = { 0.f, 0.f, 0.f, 1.f },
            },
        },
        .swapchain = sc,
    });
    sg_apply_viewport(0, 0, vp_w, vp_h, true);
    
    for(Render_Pass_Node *pn = passes->first; pn != NULL; pn = pn->next)
    {
        switch(pn->v.kind)
        {
            case Render_Pass_Kind_UI:
            {
                Render_Pass_Params_UI *params = pn->v.params_ui;
                sg_apply_pipeline(_render_sokol_state->pip_rect);
                
                // ak: calculate total bytes required across all batch groups
                size_t total_bytes = 0;
                for(Render_Batch_Group_2D_Node *gn = params->rects.first; gn != NULL; gn = gn->next)
                {
                    total_bytes += gn->batches.byte_count;
                }
                
                if(total_bytes == 0) break;
                
                // ak: Grow instance buffer if needed
                if(total_bytes > _render_sokol_state->inst_buf_size)
                {
                    sg_destroy_buffer(_render_sokol_state->inst_buf);
                    size_t ns = (total_bytes + MB(1) - 1) & ~(size_t)(MB(1) - 1);
                    _render_sokol_state->inst_buf_size = ns;
                    _render_sokol_state->inst_buf = sg_make_buffer(&(sg_buffer_desc){
                        .size  = ns,
                        .usage = {
                            .vertex_buffer   = true,
                            .write_transient = true,
                        },
                        .label = "rect_inst",
                    });
                }
                
                // ak: flatten all batch data into temporary contiguous buffer and write once to transient buffer
                {
                    Arena_Temp tmp = arena_scratch_begin(NULL, 0);
                    uint8_t *flat = arena_push(tmp.arena, uint8_t, total_bytes);
                    size_t off = 0;
                    for(Render_Batch_Group_2D_Node *gn = params->rects.first; gn != NULL; gn = gn->next)
                    {
                        for(Render_Batch_Node *bn = gn->batches.first; bn != NULL; bn = bn->next)
                        {
                            mem_copy(flat + off, bn->v.v, bn->v.byte_count);
                            off += bn->v.byte_count;
                        }
                    }
                    sg_write_buffer_transient(&(sg_write_buffer_desc){
                        .src  = { .data = (sg_range){ .ptr = flat, .size = total_bytes } },
                        .dst  = { .buffer = _render_sokol_state->inst_buf, .offset = 0 },
                        .size = total_bytes,
                    });
                    arena_scratch_end(tmp);
                }
                
                size_t cur_offset = 0;
                for(Render_Batch_Group_2D_Node *gn = params->rects.first; gn != NULL; gn = gn->next)
                {
                    Render_Batch_List *batches = &gn->batches;
                    Render_Batch_Group_2D_Params *gp = &gn->params;
                    if(batches->byte_count == 0) continue;
                    
                    // ak: resolve texture/sampler/view
                    sg_view tex_view;
                    sg_sampler tex_smp;
                    Render_Tex_2D_Format tex_fmt = Render_Tex_2D_Format_RGBA8;
                    {
                        _Render_Sokol_Tex_2D *t = HandleUnpack(_Render_Sokol_Tex_2D, gp->tex);
                        if(t && t->image.id != SG_INVALID_ID)
                        {
                            tex_fmt = t->format;
                            if(gp->tex_sample_kind == Render_Tex_2D_Sample_Kind_Linear)
                            {
                                tex_view = t->view_linear;
                                tex_smp  = t->smp_linear;
                            }
                            else
                            {
                                tex_view = t->view_nearest;
                                tex_smp  = t->smp_nearest;
                            }
                        }
                        else
                        {
                            tex_view = _render_sokol_state->white_view;
                            tex_smp  = _render_sokol_state->white_sampler;
                        }
                    }
                    
                    // ak: bindings with offset into the transient buffer
                    sg_apply_bindings(&(sg_bindings){
                        .vertex_buffers[0]        = _render_sokol_state->inst_buf,
                        .vertex_buffer_offsets[0] = (int)cur_offset,
                        .views[0]                 = tex_view,
                        .samplers[0]              = tex_smp,
                    });
                    
                    // ak: scissor
                    bool has_clip = (gp->clip.x0 != 0 || gp->clip.y0 != 0 ||
                                     gp->clip.x1 != 0 || gp->clip.y1 != 0);
                    if(has_clip)
                    {
                        int sx0 = (int)floor_f32(Clamp(0.f, gp->clip.x0, vp_dim.x));
                        int sy0 = (int)floor_f32(Clamp(0.f, gp->clip.y0, vp_dim.y));
                        int sx1 = (int) ceil_f32(Clamp(0.f, gp->clip.x1, vp_dim.x));
                        int sy1 = (int) ceil_f32(Clamp(0.f, gp->clip.y1, vp_dim.y));
                        sg_apply_scissor_rect(sx0, sy0, sx1 - sx0, sy1 - sy0, true);
                    }
                    
                    // ak: VS uniforms — 4xvec4 (64 bytes)
                    {
                        struct { float d[16]; } vs;
                        // col0
                        vs.d[0]  = gp->xform.v[0][0];
                        vs.d[1]  = gp->xform.v[0][1];
                        vs.d[2]  = gp->xform.v[0][2];
                        vs.d[3]  = 0.f;
                        // col1
                        vs.d[4]  = gp->xform.v[1][0];
                        vs.d[5]  = gp->xform.v[1][1];
                        vs.d[6]  = gp->xform.v[1][2];
                        vs.d[7]  = 0.f;
                        // col2
                        vs.d[8]  = gp->xform.v[2][0];
                        vs.d[9]  = gp->xform.v[2][1];
                        vs.d[10] = gp->xform.v[2][2];
                        vs.d[11] = 0.f;
                        // viewport
                        vs.d[12] = vp_dim.x;
                        vs.d[13] = vp_dim.y;
                        vs.d[14] = 0.f;
                        vs.d[15] = 0.f;
                        sg_apply_uniforms(0, &(sg_range){ .ptr = &vs, .size = sizeof(vs) });
                    }
                    
                    // ak: FS uniforms — mat4 channel map (64 bytes) + vec4 fs_params (16 bytes)
                    {
                        Mat4x4_F32 chan = render_sample_channel_map_from_tex2dformat(tex_fmt);
                        struct { float map[16]; float fs_params[4]; } fs;
                        mem_copy(fs.map, chan.v, sizeof(fs.map));
                        fs.fs_params[0] = 1.f - gp->transparency;
                        fs.fs_params[1] = 0.f;
                        fs.fs_params[2] = 0.f;
                        fs.fs_params[3] = 0.f;
                        sg_apply_uniforms(1, &(sg_range){ .ptr = &fs, .size = sizeof(fs) });
                    }
                    
                    int inst_count = (int)(batches->byte_count / batches->bytes_per_inst);
                    sg_draw(0, 4, inst_count);
                    
                    if(has_clip)
                    {
                        sg_apply_scissor_rect(0, 0, vp_w, vp_h, true);
                    }
                    
                    cur_offset += batches->byte_count;
                }
            } break;
            case Render_Pass_Kind_COUNT:
            default: break;
        }
    }
    
    sg_end_pass();
}

//- ak: Texture Functions ======================================================

internal Render_Handle render_tex2d_alloc(Render_Resource_Kind kind, Render_Tex_2D_Format format, Vec2_I32 size, void *data)
{
    if(!_render_sokol_state) return render_handle_zero();
    _render_sokol_ensure_initialized();
    
    sg_image_desc img_desc = {0};
    img_desc.type         = SG_IMAGETYPE_2D;
    img_desc.width        = size.x;
    img_desc.height       = size.y;
    img_desc.num_slices   = 1;
    img_desc.num_mipmaps  = 1;
    img_desc.pixel_format = _render_sokol_pixel_format(format);
    img_desc.usage.copy_dst = true;
    img_desc.label        = "user_tex2d";
    
    if(data != NULL)
    {
        size_t bpp = _render_sokol_bytes_per_pixel(format);
        img_desc.data = (sg_range){
            .ptr  = data,
            .size = (size_t)size.x * (size_t)size.y * bpp,
        };
    }
    
    sg_image img = sg_make_image(&img_desc);
    if(img.id == SG_INVALID_ID) return render_handle_zero();
    
    // ak: Recycle or allocate node
    _Render_Sokol_Tex_2D *t = _render_sokol_state->free_tex2d;
    if(t != NULL)
    {
        SLLStackPop(_render_sokol_state->free_tex2d);
        MemSetZeroStruct(t);
    }
    else
    {
        t = arena_push(_render_sokol_state->arena, _Render_Sokol_Tex_2D, 1);
    }
    
    t->image = img;
    t->smp_nearest = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
        .wrap_u     = SG_WRAP_REPEAT,
        .wrap_v     = SG_WRAP_REPEAT,
    });
    t->smp_linear = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_LINEAR,
        .mag_filter = SG_FILTER_LINEAR,
        .wrap_u     = SG_WRAP_REPEAT,
        .wrap_v     = SG_WRAP_REPEAT,
    });
    t->view_nearest = _render_sokol_texture_view(img);
    t->view_linear  = _render_sokol_texture_view(img);
    t->resource_kind = kind;
    t->format        = format;
    t->size          = size;
    
    return HandlePack(Render_Handle, t);
}

internal void render_tex2d_free(Render_Handle handle)
{
    if(!_render_sokol_state) return;
    _Render_Sokol_Tex_2D *t = HandleUnpack(_Render_Sokol_Tex_2D, handle);
    if(!t) return;
    if(t->view_nearest.id != SG_INVALID_ID) sg_destroy_view(t->view_nearest);
    if(t->view_linear.id  != SG_INVALID_ID) sg_destroy_view(t->view_linear);
    if(t->image.id        != SG_INVALID_ID) sg_destroy_image(t->image);
    if(t->smp_nearest.id  != SG_INVALID_ID) sg_destroy_sampler(t->smp_nearest);
    if(t->smp_linear.id   != SG_INVALID_ID) sg_destroy_sampler(t->smp_linear);
    MemSetZeroStruct(t);
    SLLStackPush(_render_sokol_state->free_tex2d, t);
}

internal void render_fill_tex2d_region(Render_Handle texture, Rng2_I32 subrect, void *data)
{
    if(!_render_sokol_state || !data) return;
    _Render_Sokol_Tex_2D *tex = HandleUnpack(_Render_Sokol_Tex_2D, texture);
    if(!tex || tex->image.id == SG_INVALID_ID) return;
    
    Vec2_I32 rect_size = dim_rng2(subrect);
    if(rect_size.x <= 0 || rect_size.y <= 0) return;
    
    size_t bpp = _render_sokol_bytes_per_pixel(tex->format);
    size_t bytes_per_row = (size_t)rect_size.x * bpp;
    size_t total_bytes = bytes_per_row * (size_t)rect_size.y;
    
    // ak: grow staging buffer if needed
    if(total_bytes > _render_sokol_state->staging_buf_size)
    {
        if(_render_sokol_state->staging_buf.id != SG_INVALID_ID)
        {
            sg_destroy_buffer(_render_sokol_state->staging_buf);
        }
        size_t ns = (total_bytes + MB(1) - 1) & ~(size_t)(MB(1) - 1);
        _render_sokol_state->staging_buf_size = ns;
        _render_sokol_state->staging_buf = sg_make_buffer(&(sg_buffer_desc){
            .size = ns,
            .usage = {
                .staging_buffer  = true,
                .write_transient = true,
                .copy_src        = true,
            },
            .label = "tex_staging",
        });
    }
    
    sg_write_buffer_transient(&(sg_write_buffer_desc){
        .src = { .data = (sg_range){ .ptr = data, .size = total_bytes } },
        .dst = { .buffer = _render_sokol_state->staging_buf, .offset = 0 },
        .size = total_bytes,
    });
    
    sg_copy_buffer_to_image(&(sg_copy_buffer_to_image_desc){
        .src = {
            .buffer = _render_sokol_state->staging_buf,
            .offset = 0,
            .bytes_per_row = (int)bytes_per_row,
            .bytes_per_slice = (int)total_bytes,
        },
        .dst = {
            .image = tex->image,
            .mip_level = 0,
            .x = subrect.x0,
            .y = subrect.y0,
            .slice = 0,
        },
        .size = {
            .width = rect_size.x,
            .height = rect_size.y,
            .num_slices = 1,
        },
    });
}

