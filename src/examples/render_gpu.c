#define GPU_IMPLEMENTATION
#include "external/gpu.h"
#include "render_gpu_spv.h"

// ak: Helpers
//=============================================================================

internal Vec2_I32 _render_gpu_vec2_i32(int32_t x, int32_t y)
{
    Vec2_I32 r;
    r.x = x;
    r.y = y;
    return r;
}

internal Vec4_F32 _render_gpu_vec4_f32(float x, float y, float z, float w)
{
    Vec4_F32 r;
    r.x = x;
    r.y = y;
    r.z = z;
    r.w = w;
    return r;
}

internal Render_Handle _render_gpu_handle_from_tex2d(Render_Gpu_Tex_2D *tex)
{
    Render_Handle handle = { (uint64_t)tex };
    return handle;
}

internal Render_Gpu_Tex_2D *_render_gpu_tex2d_from_handle(Render_Handle handle)
{
    Render_Gpu_Tex_2D *tex = (Render_Gpu_Tex_2D *)handle.u64[0];
    return tex;
}

internal GPU_Format _render_gpu_format_from_tex2d_format(Render_Tex_2D_Format format)
{
    GPU_Format result = GPU_Format_R8G8B8A8_UNorm;
    switch (format)
    {
        case Render_Tex_2D_Format_R8:     result = GPU_Format_R8_UNorm; break;
        case Render_Tex_2D_Format_RG8:    result = GPU_Format_R8G8_UNorm; break;
        case Render_Tex_2D_Format_RGBA8:  result = GPU_Format_R8G8B8A8_UNorm; break;
        case Render_Tex_2D_Format_BGRA8:  result = GPU_Format_B8G8R8A8_UNorm; break;
        case Render_Tex_2D_Format_R16:    result = GPU_Format_R16_Float; break;
        case Render_Tex_2D_Format_RGBA16: result = GPU_Format_R16G16B16A16_Float; break;
        case Render_Tex_2D_Format_R32:    result = GPU_Format_R32_Float; break;
        case Render_Tex_2D_Format_RG32:   result = GPU_Format_R32G32_Float; break;
        case Render_Tex_2D_Format_RGBA32: result = GPU_Format_R32G32B32A32_Float; break;
        default: break;
    }
    return result;
}

internal void _render_gpu_slot_ensure_capacity(Render_Gpu_FrameSlot *slot, U64 need_bytes)
{
    if (slot->dynamic_buffer == NULL)
    {
        slot->dynamic_capacity = GPU_Max(MB(4), need_bytes);
        slot->dynamic_buffer = gpu_malloc(_render_gpu_state->device, slot->dynamic_capacity, GPU_MemoryKind_Default);
        slot->dynamic_offset = 0;
    }
    else if (slot->dynamic_offset + need_bytes > slot->dynamic_capacity)
    {
        Render_Gpu_ScratchAlloc *scratch = arena_push(_render_gpu_state->arena, Render_Gpu_ScratchAlloc, 1);
        scratch->alloc = slot->dynamic_buffer;
        SLLStackPush(_render_gpu_state->scratch_allocs, scratch);

        slot->dynamic_capacity = GPU_Max(slot->dynamic_capacity * 2, slot->dynamic_offset + need_bytes + MB(1));
        slot->dynamic_buffer = gpu_malloc(_render_gpu_state->device, slot->dynamic_capacity, GPU_MemoryKind_Default);
        slot->dynamic_offset = 0;
    }
}

internal GPU_WindowHandle _render_gpu_window_handle_from_wl(Wl_Window window)
{
    GPU_WindowHandle win_handle = STRUCT_ZERO;
#if OS_LINUX
    _Wl_X11_Window *window_os = (_Wl_X11_Window *)window.u64[0];
    win_handle.connection = _wl_x11_state->connection;
    win_handle.window = window_os->xwindow;
#elif OS_WINDOWS
    _Wl_Win32_State *state = _wl_win32_state;
    win_handle.hinstance = state->instance;
    win_handle.hwnd = state->handle;
#endif
    return win_handle;
}

// ak: Backend Hooks Implementation
//=============================================================================

internal void render_init(void)
{
    Arena *arena = arena_alloc();
    _render_gpu_state = arena_push(arena, Render_Gpu_State, 1);
    _render_gpu_state->arena = arena;

    GPU_DeviceDesc desc = STRUCT_ZERO;
    desc.enable_surface = 1;
    desc.app_name = "CMono";
#if BUILD_DEBUG
    desc.enable_validation = 1;
#endif

    _render_gpu_state->device = gpu_device_create(&desc);
    if (!_render_gpu_state->device)
    {
        LogErrorLine(&_os_core_state.log_context, "Failed to create GPU device.");
        os_exit(1);
    }
    _render_gpu_state->queue = gpu_queue_create(_render_gpu_state->device);

    _render_gpu_state->transfer_buffer_size = MB(4);
    _render_gpu_state->transfer_buffer = gpu_malloc(_render_gpu_state->device, _render_gpu_state->transfer_buffer_size, GPU_MemoryKind_Default);

    uint32_t white_pixel = 0xffffffff;
    _render_gpu_state->white_texture = render_tex2d_alloc(
        Render_Resource_Kind_Static,
        Render_Tex_2D_Format_RGBA8,
        _render_gpu_vec2_i32(1, 1),
        &white_pixel
    );
}

internal void render_cleanup(void)
{
    if (_render_gpu_state)
    {
        if (_render_gpu_state->device)
        {
            gpu_device_wait_idle(_render_gpu_state->device);
            render_tex2d_free(_render_gpu_state->white_texture);
            if (_render_gpu_state->transfer_buffer)
            {
                gpu_free(_render_gpu_state->device, _render_gpu_state->transfer_buffer);
                _render_gpu_state->transfer_buffer = NULL;
            }
            if (_render_gpu_state->pipeline_rect)
            {
                gpu_pipeline_destroy(_render_gpu_state->device, _render_gpu_state->pipeline_rect);
                _render_gpu_state->pipeline_rect = NULL;
            }
            if (_render_gpu_state->queue)
            {
                gpu_queue_destroy(_render_gpu_state->device, _render_gpu_state->queue);
                _render_gpu_state->queue = NULL;
            }
            gpu_device_destroy(_render_gpu_state->device);
            _render_gpu_state->device = NULL;
        }
        if (_render_gpu_state->arena)
        {
            arena_free(_render_gpu_state->arena);
        }
        _render_gpu_state = NULL;
    }
}

internal Render_Handle render_window_equip(Wl_Window window)
{
    Render_Gpu_Window *w = _render_gpu_state->free_window;
    if (w)
    {
        SLLStackPop(_render_gpu_state->free_window);
    }
    else
    {
        w = arena_push(_render_gpu_state->arena, Render_Gpu_Window, 1);
    }
    MemSetZeroStruct(w);
    w->window = window;

    Rng2_F32 canvas_rect = wl_canvas_rect_from_window(window);
    Vec2_F32 canvas_rect_dim = dim_rng2(canvas_rect);
    U32 width = (U32)GPU_Max((U32)canvas_rect_dim.x, 1);
    U32 height = (U32)GPU_Max((U32)canvas_rect_dim.y, 1);
    w->last_canvas_rect_dim = canvas_rect_dim;

    GPU_WindowHandle win_handle = _render_gpu_window_handle_from_wl(window);
    w->swapchain = gpu_swapchain_create(_render_gpu_state->device, &win_handle, width, height);
    if (!w->swapchain)
    {
        LogErrorLine(&_os_core_state.log_context, "Failed to create GPU swapchain.");
        os_exit(1);
    }

    if (!_render_gpu_state->pipeline_rect)
    {
        GPU_RasterDesc rdesc = STRUCT_ZERO;
        rdesc.color_format = gpu_swapchain_format(w->swapchain);
        rdesc.depth_format = GPU_Format_Undefined;
        rdesc.blend_enable = 1;
        rdesc.cull_mode = GPU_CullMode_None;
        _render_gpu_state->pipeline_rect = gpu_pipeline_graphics_create(
            _render_gpu_state->device,
            (const U8 *)_render_gpu_rect_vert_spv, sizeof(_render_gpu_rect_vert_spv),
            (const U8 *)_render_gpu_rect_frag_spv, sizeof(_render_gpu_rect_frag_spv),
            &rdesc
        );
    }

    for (U32 i = 0; i < w->swapchain->image_count; i++)
    {
        _render_gpu_slot_ensure_capacity(&w->frame_slots[i], MB(4));
    }

    Render_Handle result = { (uint64_t)w };
    return result;
}

internal void render_window_unequip(Wl_Window window, Render_Handle window_equip)
{
    Unused(window);
    Render_Gpu_Window *w = (Render_Gpu_Window *)window_equip.u64[0];
    if (w)
    {
        gpu_device_wait_idle(_render_gpu_state->device);
        if (w->swapchain)
        {
            for (U32 i = 0; i < w->swapchain->image_count; i++)
            {
                if (w->frame_slots[i].dynamic_buffer)
                {
                    gpu_free(_render_gpu_state->device, w->frame_slots[i].dynamic_buffer);
                    w->frame_slots[i].dynamic_buffer = NULL;
                }
            }
            gpu_swapchain_destroy(_render_gpu_state->device, w->swapchain);
            w->swapchain = NULL;
        }
        SLLStackPush(_render_gpu_state->free_window, w);
    }
}

internal void render_begin_frame(void)
{
}

internal void render_end_frame(void)
{
    for (Render_Gpu_ScratchAlloc *s = _render_gpu_state->scratch_allocs; s != NULL; s = s->next)
    {
        if (s->alloc)
        {
            gpu_free(_render_gpu_state->device, s->alloc);
            s->alloc = NULL;
        }
    }
    _render_gpu_state->scratch_allocs = NULL;
}

internal void render_window_begin_frame(Wl_Window window, Render_Handle handle)
{
    Render_Gpu_Window *w = (Render_Gpu_Window *)handle.u64[0];
    Rng2_F32 canvas_rect = wl_canvas_rect_from_window(window);
    Vec2_F32 canvas_rect_dim = dim_rng2(canvas_rect);
    U32 width = (U32)GPU_Max((U32)canvas_rect_dim.x, 1);
    U32 height = (U32)GPU_Max((U32)canvas_rect_dim.y, 1);

    if ((U32)canvas_rect_dim.x != (U32)w->last_canvas_rect_dim.x ||
        (U32)canvas_rect_dim.y != (U32)w->last_canvas_rect_dim.y)
    {
        w->last_canvas_rect_dim = canvas_rect_dim;
        gpu_device_wait_idle(_render_gpu_state->device);
        if (w->swapchain)
        {
            gpu_swapchain_destroy(_render_gpu_state->device, w->swapchain);
        }
        GPU_WindowHandle win_handle = _render_gpu_window_handle_from_wl(window);
        w->swapchain = gpu_swapchain_create(_render_gpu_state->device, &win_handle, width, height);
        for (U32 i = 0; i < w->swapchain->image_count; i++)
        {
            _render_gpu_slot_ensure_capacity(&w->frame_slots[i], MB(4));
        }
    }

    w->current_target = gpu_swapchain_acquire(_render_gpu_state->device, w->swapchain);
    if (!w->current_target)
    {
        gpu_device_wait_idle(_render_gpu_state->device);
        if (w->swapchain)
        {
            gpu_swapchain_destroy(_render_gpu_state->device, w->swapchain);
        }
        GPU_WindowHandle win_handle = _render_gpu_window_handle_from_wl(window);
        w->swapchain = gpu_swapchain_create(_render_gpu_state->device, &win_handle, width, height);
        for (U32 i = 0; i < w->swapchain->image_count; i++)
        {
            _render_gpu_slot_ensure_capacity(&w->frame_slots[i], MB(4));
        }
        w->current_target = gpu_swapchain_acquire(_render_gpu_state->device, w->swapchain);
    }

    U32 slot_idx = w->swapchain->current_frame;
    w->frame_slots[slot_idx].dynamic_offset = 0;

    w->current_cb = gpu_swapchain_cmd_buf(w->swapchain);
    gpu_cmd_begin_rendering(w->current_cb, w->current_target, NULL, width, height);
    gpu_cmd_set_viewport(w->current_cb, 0, 0, (float)width, (float)height, 0.0f, 1.0f);
    gpu_cmd_set_scissor(w->current_cb, 0, 0, width, height);
}

internal void render_window_end_frame(Wl_Window window, Render_Handle handle)
{
    Unused(window);
    Render_Gpu_Window *w = (Render_Gpu_Window *)handle.u64[0];
    if (w->current_cb)
    {
        gpu_cmd_end_rendering(w->current_cb);
        gpu_swapchain_submit_and_present(_render_gpu_state->device, _render_gpu_state->queue, w->swapchain, w->current_cb);
        w->current_cb = NULL;
        w->current_target = NULL;
    }
}

internal void render_window_submit(Wl_Window window, Render_Handle window_equip, Render_Pass_List *passes)
{
    Unused(window);
    Render_Gpu_Window *w = (Render_Gpu_Window *)window_equip.u64[0];
    if (!w->current_cb || !w->current_target) return;

    GPU_CmdBuf *cb = w->current_cb;
    U32 slot_idx = w->swapchain->current_frame;
    Render_Gpu_FrameSlot *slot = &w->frame_slots[slot_idx];

    gpu_cmd_set_pipeline(cb, _render_gpu_state->pipeline_rect);

    for (Render_Pass_Node *pass_n = passes->first; pass_n != 0; pass_n = pass_n->next)
    {
        Render_Pass *pass = &pass_n->v;
        switch (pass->kind)
        {
            case Render_Pass_Kind_UI:
            {
                Render_Pass_Params_UI *params = pass->params_ui;
                Render_Batch_Group_2D_List *rect_batch_groups = &params->rects;

                for (Render_Batch_Group_2D_Node *group_n = rect_batch_groups->first; group_n != 0; group_n = group_n->next)
                {
                    Render_Batch_List *batches = &group_n->batches;
                    Render_Batch_Group_2D_Params *group_params = &group_n->params;

                    if (batches->byte_count == 0) continue;

                    Render_Tex_2D_Format texture_fmt = Render_Tex_2D_Format_RGBA8;
                    GPU_Texture *texture = NULL;
                    Render_Gpu_Tex_2D *tex = _render_gpu_tex2d_from_handle(group_params->tex);
                   if (tex && tex->texture)
                    {
                        texture = tex->texture;
                        texture_fmt = tex->format;
                    }
                    else
                    {
                        Render_Gpu_Tex_2D *white = _render_gpu_tex2d_from_handle(_render_gpu_state->white_texture);
                        if (white)
                        {
                            texture = white->texture;
                        }
                    }

                    // Instances
                    U64 inst_size = batches->byte_count;
                    U64 inst_alloc_size = GPU_AlignUp(inst_size, 64);
                    _render_gpu_slot_ensure_capacity(slot, inst_alloc_size + 256);
                    void *inst_dst = (uint8_t *)gpu_alloc_cpu_ptr(slot->dynamic_buffer) + slot->dynamic_offset;
                    U64 instance_gpu_addr = slot->dynamic_buffer->device_addr + slot->dynamic_offset;
                    slot->dynamic_offset += inst_alloc_size;

                    size_t off = 0;
                    for (Render_Batch_Node *bn = batches->first; bn != 0; bn = bn->next)
                    {
                        memcpy((uint8_t *)inst_dst + off, bn->v.v, bn->v.byte_count);
                        off += bn->v.byte_count;
                    }

                    // Uniforms
                    Render_Gpu_Uniforms uniforms = STRUCT_ZERO;
                    uniforms.texture_sample_channel_map = render_sample_channel_map_from_tex2dformat(texture_fmt);
                    uniforms.xform_col0 = _render_gpu_vec4_f32(group_params->xform.v[0][0], group_params->xform.v[0][1], group_params->xform.v[0][2], 0.0f);
                    uniforms.xform_col1 = _render_gpu_vec4_f32(group_params->xform.v[1][0], group_params->xform.v[1][1], group_params->xform.v[1][2], 0.0f);
                    uniforms.xform_col2 = _render_gpu_vec4_f32(group_params->xform.v[2][0], group_params->xform.v[2][1], group_params->xform.v[2][2], 0.0f);
                    uniforms.viewport_size_px = w->last_canvas_rect_dim;
                    uniforms.opacity = 1.0f - group_params->transparency;

                    U64 unif_alloc_size = GPU_AlignUp(sizeof(uniforms), 64);
                    _render_gpu_slot_ensure_capacity(slot, unif_alloc_size);
                    void *unif_dst = (uint8_t *)gpu_alloc_cpu_ptr(slot->dynamic_buffer) + slot->dynamic_offset;
                    U64 uniform_gpu_addr = slot->dynamic_buffer->device_addr + slot->dynamic_offset;
                    slot->dynamic_offset += unif_alloc_size;
                    memcpy(unif_dst, &uniforms, sizeof(uniforms));

                    // Bind texture
                    U32 sample_kind = (group_params->tex_sample_kind == Render_Tex_2D_Sample_Kind_Linear) ? 1 : 0;
                    gpu_cmd_bind_texture(cb, texture, sample_kind);

                    // Scissor
                    bool has_clip = (group_params->clip.x0 != 0 || group_params->clip.x1 != 0 ||
                                     group_params->clip.y0 != 0 || group_params->clip.y1 != 0);
                    if (has_clip)
                    {
                        int32_t x0 = Clamp(0, (int32_t)floor_f32(group_params->clip.x0), (int32_t)w->last_canvas_rect_dim.x);
                        int32_t y0 = Clamp(0, (int32_t)floor_f32(group_params->clip.y0), (int32_t)w->last_canvas_rect_dim.y);
                        int32_t x1 = Clamp(0, (int32_t)ceil_f32(group_params->clip.x1), (int32_t)w->last_canvas_rect_dim.x);
                        int32_t y1 = Clamp(0, (int32_t)ceil_f32(group_params->clip.y1), (int32_t)w->last_canvas_rect_dim.y);
                        int32_t swidth = x1 - x0;
                        int32_t sheight = y1 - y0;
                        if (swidth <= 0 || sheight <= 0) continue;
                        gpu_cmd_set_scissor(cb, x0, y0, (U32)swidth, (U32)sheight);
                    }
                    else
                    {
                        gpu_cmd_set_scissor(cb, 0, 0, (U32)w->last_canvas_rect_dim.x, (U32)w->last_canvas_rect_dim.y);
                    }

                    // Draw
                    U32 inst_count = (U32)(batches->byte_count / batches->bytes_per_inst);
                    gpu_cmd_draw(cb, instance_gpu_addr, uniform_gpu_addr, 6, inst_count);

                    if (has_clip)
                    {
                        gpu_cmd_set_scissor(cb, 0, 0, (U32)w->last_canvas_rect_dim.x, (U32)w->last_canvas_rect_dim.y);
                    }
                }
            } break;

            case Render_Pass_Kind_COUNT: break;
        }
    }
}

internal Render_Handle render_tex2d_alloc(Render_Resource_Kind kind, Render_Tex_2D_Format format, Vec2_I32 size, void *data)
{
    Render_Gpu_Tex_2D *tex = _render_gpu_state->free_tex2d;
    if (tex)
    {
        SLLStackPop(_render_gpu_state->free_tex2d);
    }
    else
    {
        tex = arena_push(_render_gpu_state->arena, Render_Gpu_Tex_2D, 1);
    }
    MemSetZeroStruct(tex);
    tex->resource_kind = kind;
    tex->format = format;
    tex->size = size;

    GPU_TextureDesc desc = STRUCT_ZERO;
    desc.width = (U32)size.x;
    desc.height = (U32)size.y;
    desc.depth = 1;
    desc.mip_levels = 1;
    desc.format = _render_gpu_format_from_tex2d_format(format);

    tex->texture = gpu_texture_create(_render_gpu_state->device, &desc, NULL);

    if (data != NULL)
    {
        U64 bpp = _render_tex2d_format_bytes_per_pixel_table[format];
        U64 size_bytes = (U64)size.x * (U64)size.y * bpp;
        GPU_Alloc *upload = NULL;
        B32 need_free = 0;
        if (_render_gpu_state->transfer_buffer && size_bytes <= _render_gpu_state->transfer_buffer_size)
        {
            upload = _render_gpu_state->transfer_buffer;
        }
        else
        {
            upload = gpu_malloc(_render_gpu_state->device, size_bytes, GPU_MemoryKind_Default);
            need_free = 1;
        }
        memcpy(gpu_alloc_cpu_ptr(upload), data, size_bytes);

        GPU_CmdBuf *cb = gpu_cmd_begin(_render_gpu_state->device, _render_gpu_state->queue);
        gpu_cmd_copy_to_texture(cb, tex->texture, upload);
        gpu_submit(_render_gpu_state->device, _render_gpu_state->queue, &cb, 1, NULL, 0);
        gpu_queue_wait_idle(_render_gpu_state->device, _render_gpu_state->queue);

        if (need_free)
        {
            gpu_free(_render_gpu_state->device, upload);
        }
    }

    Render_Handle result = { (uint64_t)tex };
    return result;
}

internal void render_tex2d_free(Render_Handle handle)
{
    Render_Gpu_Tex_2D *tex = _render_gpu_tex2d_from_handle(handle);
    if (tex)
    {
        if (tex->texture)
        {
            gpu_texture_destroy(_render_gpu_state->device, tex->texture);
            tex->texture = NULL;
        }
        SLLStackPush(_render_gpu_state->free_tex2d, tex);
    }
}

internal void render_fill_tex2d_region(Render_Handle texture, Rng2_I32 subrect, void *data)
{
    Render_Gpu_Tex_2D *tex = _render_gpu_tex2d_from_handle(texture);
    if (tex && tex->texture && data)
    {
        Vec2_I32 rect_size = dim_rng2(subrect);
        if (rect_size.x <= 0 || rect_size.y <= 0) return;

        U64 bpp = _render_tex2d_format_bytes_per_pixel_table[tex->format];
        U64 size_bytes = (U64)rect_size.x * (U64)rect_size.y * bpp;

        GPU_Alloc *upload = NULL;
        B32 need_free = 0;
        if (_render_gpu_state->transfer_buffer && size_bytes <= _render_gpu_state->transfer_buffer_size)
        {
            upload = _render_gpu_state->transfer_buffer;
        }
        else
        {
            upload = gpu_malloc(_render_gpu_state->device, size_bytes, GPU_MemoryKind_Default);
            need_free = 1;
        }
        memcpy(gpu_alloc_cpu_ptr(upload), data, size_bytes);

        GPU_CmdBuf *cb = gpu_cmd_begin(_render_gpu_state->device, _render_gpu_state->queue);
        gpu_cmd_copy_to_texture_region(cb, tex->texture, upload, (U32)subrect.x0, (U32)subrect.y0, (U32)rect_size.x, (U32)rect_size.y);
        gpu_submit(_render_gpu_state->device, _render_gpu_state->queue, &cb, 1, NULL, 0);
        gpu_queue_wait_idle(_render_gpu_state->device, _render_gpu_state->queue);

        if (need_free)
        {
            gpu_free(_render_gpu_state->device, upload);
        }
    }
}
