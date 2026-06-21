internal Draw_Bucket *draw_bucket_make(void)
{
    Draw_Bucket *bucket = arena_push(_draw_contex->arena, Draw_Bucket, 1);
    bucket->top_tex_2d_sample_kind = &draw_nil_tex_2d_sample_kind;
    bucket->top_xform_2d = &draw_nil_xform_2d;
    bucket->top_clip = &draw_nil_clip;
    bucket->top_transparency = &draw_nil_transparency;
    return bucket;
}

internal void draw_begin_frame(void)
{
    if(_draw_contex == NULL)
    {
        Arena *arena = arena_alloc();
        _draw_contex = arena_push(arena, _Draw_Contex, 1);
        _draw_contex->arena = arena;
        _draw_contex->arena_frame_start_pos = arena_pos(arena);
    }
    arena_pop_to(_draw_contex->arena, _draw_contex->arena_frame_start_pos);
    _draw_contex->free_bucket_selection = NULL;
    _draw_contex->top_bucket = NULL;
}

internal void draw_bucket_push(Draw_Bucket *bucket)
{
    _Draw_Bucket_Selection_Node *node = _draw_contex->free_bucket_selection;
    if(node)
    {
        SLLStackPop(_draw_contex->free_bucket_selection);
    }
    else
    {
        node = arena_push(_draw_contex->arena, _Draw_Bucket_Selection_Node, 1);
    }
    SLLStackPush(_draw_contex->top_bucket, node);
    node->bucket = bucket;
}

internal void draw_bucket_pop(void)
{
  _Draw_Bucket_Selection_Node *node = _draw_contex->top_bucket;
  SLLStackPop(_draw_contex->top_bucket);
  SLLStackPush(_draw_contex->free_bucket_selection, node);
}

internal Draw_Bucket *draw_bucket_top(void)
{
    Draw_Bucket *bucket = NULL;
    if(_draw_contex->top_bucket != NULL)
    {
        bucket = _draw_contex->top_bucket->bucket;
    }
    return bucket;
}

internal inline Render_Rect_2D_Inst *dr_rect(Rng2_F32 dst, Vec4_F32 color, float corner_radius, float border_thickness, float edge_softness)
{
    Arena *arena = _draw_contex->arena;
    Draw_Bucket *bucket = draw_bucket_top();
    Render_Pass *pass = render_pass_from_kind(arena, &bucket->passes, Render_Pass_Kind_UI);
    Render_Pass_Params_UI *params = pass->params_ui;
    Render_Batch_Group_2D_List *rects = &params->rects;
    Render_Batch_Group_2D_Node *node = rects->last;
    if(node == NULL || bucket->stack_gen != bucket->last_cmd_stack_gen)
    {
        node = arena_push(arena, Render_Batch_Group_2D_Node, 1);
        SLLQueuePush(rects->first, rects->last, node);
        rects->count += 1;
        node->batches = render_batch_list_make(sizeof(Render_Rect_2D_Inst));
        node->params.tex = render_handle_zero();
        node->params.tex_sample_kind = bucket->top_tex_2d_sample_kind->v;
        node->params.xform           = bucket->top_xform_2d->v;
        node->params.clip            = bucket->top_clip->v;
        node->params.transparency    = bucket->top_transparency->v;
    }
    Render_Rect_2D_Inst *inst = (Render_Rect_2D_Inst *)render_batch_list_push_inst(arena, &node->batches, 256);
    inst->dst = dst;
    inst->src = rng2p(.0f, .0f, .0f, .0f);
    inst->colors[Corner_TopLeft] = color;
    inst->colors[Corner_BottomLeft] = color;
    inst->colors[Corner_TopRight] = color;
    inst->colors[Corner_BottomRight] = color;
    inst->corner_radii[Corner_TopLeft] = corner_radius;
    inst->corner_radii[Corner_BottomLeft] = corner_radius;
    inst->corner_radii[Corner_TopRight] = corner_radius;
    inst->corner_radii[Corner_BottomRight] = corner_radius;
    inst->border_thickness = border_thickness;
    inst->edge_softness = edge_softness;
    inst->white_texture_override = 1.f;
    inst->shear = 0.f;
    bucket->last_cmd_stack_gen = bucket->stack_gen;
    return inst;
}

internal void draw_submit_bucket(Wl_Window window, Render_Handle r_window, Draw_Bucket *bucket)
{
    render_window_submit(window, r_window, &bucket->passes);
}
