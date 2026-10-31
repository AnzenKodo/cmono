#ifndef RENDER_SOKOL_H
#define RENDER_SOKOL_H

#ifndef SOKOL_GLCORE
#define SOKOL_GLCORE
#endif

#pragma push_macro("internal")
#undef internal
#include "vendor/sokol_gfx.h"
#pragma pop_macro("internal")

// ak: Types
//=============================================================================

typedef struct _Render_Sokol_Tex_2D _Render_Sokol_Tex_2D;
struct _Render_Sokol_Tex_2D
{
    _Render_Sokol_Tex_2D  *next;
    _Render_Sokol_Tex_2D  *prev;
    sg_image              image;
    sg_view               view_nearest;   // texture view for nearest sampling
    sg_view               view_linear;    // texture view for linear sampling
    sg_sampler            smp_nearest;
    sg_sampler            smp_linear;
    Render_Resource_Kind  resource_kind;
    Render_Tex_2D_Format  format;
    Vec2_I32              size;
};

typedef struct _Render_Sokol_Window _Render_Sokol_Window;
struct _Render_Sokol_Window
{
    _Render_Sokol_Window *next;
    _Render_Sokol_Window *prev;
    Wl_Window             window;
};

typedef struct _Render_Sokol_State _Render_Sokol_State;
struct _Render_Sokol_State
{
    Arena                *arena;
    bool                  initialized;

    // White 1x1 fallback texture
    sg_image              white_image;
    sg_view               white_view;
    sg_sampler            white_sampler;

    // Shaders & Pipelines
    sg_shader             shd_rect;
    sg_pipeline           pip_rect;

    // Shared transient instance buffer for rect draws
    sg_buffer             inst_buf;
    size_t                inst_buf_size;

    // Staging buffer for sub-image updates
    sg_buffer             staging_buf;
    size_t                staging_buf_size;

    // Free-lists
    _Render_Sokol_Tex_2D  *free_tex2d;
    _Render_Sokol_Window  *free_window;
};

// ak: Function prototypes
//=============================================================================

internal sg_pixel_format _render_sokol_pixel_format(Render_Tex_2D_Format fmt);
internal size_t          _render_sokol_bytes_per_pixel(Render_Tex_2D_Format format);

internal Render_Handle        _render_handle_from_sokol_tex2d(_Render_Sokol_Tex_2D *t);
internal _Render_Sokol_Tex_2D *_render_sokol_tex2d_from_handle(Render_Handle h);

internal sg_view     _render_sokol_texture_view(sg_image image);
internal sg_pipeline _render_sokol_make_rect_pipeline(void);
internal void        _render_sokol_ensure_initialized(void);

// ak: Global state
//=============================================================================

global _Render_Sokol_State *_render_sokol_state = STRUCT_ZERO;

#endif // RENDER_SOKOL_H
