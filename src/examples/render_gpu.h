#ifndef RENDER_GPU_H
#define RENDER_GPU_H

// ak: External Includes
//=============================================================================

#include "external/gpu.h"

// ak: Types
//=============================================================================

typedef struct Render_Gpu_Uniforms Render_Gpu_Uniforms;
struct Render_Gpu_Uniforms
{
    Mat4x4_F32 texture_sample_channel_map;
    Vec4_F32 xform_col0;
    Vec4_F32 xform_col1;
    Vec4_F32 xform_col2;
    Vec2_F32 viewport_size_px;
    float opacity;
    float padding;
};

typedef struct Render_Gpu_Tex_2D Render_Gpu_Tex_2D;
struct Render_Gpu_Tex_2D
{
    Render_Gpu_Tex_2D *next;
    Render_Gpu_Tex_2D *prev;
    GPU_Texture *texture;
    Render_Resource_Kind resource_kind;
    Render_Tex_2D_Format format;
    Vec2_I32 size;
};

typedef struct Render_Gpu_FrameSlot Render_Gpu_FrameSlot;
struct Render_Gpu_FrameSlot
{
    GPU_Alloc *dynamic_buffer;
    U64 dynamic_offset;
    U64 dynamic_capacity;
};

typedef struct Render_Gpu_Window Render_Gpu_Window;
struct Render_Gpu_Window
{
    Render_Gpu_Window *next;
    Wl_Window window;
    GPU_Swapchain *swapchain;
    GPU_CmdBuf *current_cb;
    GPU_Texture *current_target;
    Vec2_F32 last_canvas_rect_dim;
    Render_Gpu_FrameSlot frame_slots[8];
};

typedef struct Render_Gpu_ScratchAlloc Render_Gpu_ScratchAlloc;
struct Render_Gpu_ScratchAlloc
{
    Render_Gpu_ScratchAlloc *next;
    GPU_Alloc *alloc;
};

typedef struct Render_Gpu_State Render_Gpu_State;
struct Render_Gpu_State
{
    Arena *arena;
    GPU_Device *device;
    GPU_Queue *queue;
    GPU_Pipeline *pipeline_rect;
    Render_Handle white_texture;
    Render_Gpu_Window *free_window;
    Render_Gpu_Tex_2D *free_tex2d;
    Render_Gpu_ScratchAlloc *scratch_allocs;
    GPU_Alloc *transfer_buffer;
    U64 transfer_buffer_size;
};

// ak: Global Variables
//=============================================================================

global Render_Gpu_State *_render_gpu_state = NULL;

#endif // RENDER_GPU_H
