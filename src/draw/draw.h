#ifndef DRAW_H
#define DRAW_H

typedef struct _Draw_Tex_2D_Sample_Kind_Node _Draw_Tex_2D_Sample_Kind_Node;
struct _Draw_Tex_2D_Sample_Kind_Node
{
    _Draw_Tex_2D_Sample_Kind_Node *next;
    Render_Tex_2D_Sample_Kind v;
};

typedef struct _Draw_XForm_2D_Node _Draw_XForm_2D_Node;
struct _Draw_XForm_2D_Node
{
    _Draw_XForm_2D_Node *next;
    Mat3x3_F32 v;
};

typedef struct _Draw_Clip_Node _Draw_Clip_Node;
struct _Draw_Clip_Node
{
    _Draw_Clip_Node *next;
    Rng2_F32 v;
};

typedef struct _Draw_Transparency_Node _Draw_Transparency_Node;
struct _Draw_Transparency_Node
{
    _Draw_Transparency_Node *next;
    float v;
};

typedef struct Draw_Bucket Draw_Bucket;
struct Draw_Bucket
{
    Render_Pass_List passes;
    size_t stack_gen;
    size_t last_cmd_stack_gen;
    _Draw_Tex_2D_Sample_Kind_Node *top_tex_2d_sample_kind;
    _Draw_XForm_2D_Node *top_xform_2d;
    _Draw_Clip_Node *top_clip;
    _Draw_Transparency_Node *top_transparency;
};

typedef struct _Draw_Bucket_Selection_Node _Draw_Bucket_Selection_Node;
struct _Draw_Bucket_Selection_Node
{
    _Draw_Bucket_Selection_Node *next;
    Draw_Bucket *bucket;
};

typedef struct _Draw_Contex _Draw_Contex;
struct _Draw_Contex
{
    Arena *arena;
    size_t arena_frame_start_pos;
    _Draw_Bucket_Selection_Node *free_bucket_selection;
    _Draw_Bucket_Selection_Node *top_bucket;
};


internal Draw_Bucket *draw_bucket_make(void);
internal void draw_begin_frame(void);

internal void draw_bucket_push(Draw_Bucket *bucket);
internal void draw_bucket_pop(void);
internal Draw_Bucket *draw_bucket_top(void);
#define DrawBucketScope(b) DeferLoop(draw_bucket_push(b), draw_bucket_pop())

global _Draw_Contex *_draw_contex = 0;
read_only global _Draw_Tex_2D_Sample_Kind_Node draw_nil_tex_2d_sample_kind = {
    0, Render_Tex_2D_Sample_Kind_Nearest
};
read_only global _Draw_XForm_2D_Node draw_nil_xform_2d = {
    0, { 1, 0, 0, 0, 1, 0, 0, 0, 1 }
};
read_only global _Draw_Clip_Node draw_nil_clip = { 0, { 0 } };
read_only global _Draw_Transparency_Node draw_nil_transparency = { 0, 0 };

#endif // DRAW_H
