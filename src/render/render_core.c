//~ ak: Helper functions
//=============================================================================

internal Render_Handle render_handle_zero(void)
{
    Render_Handle handle = ZERO_STRUCT;
    return handle;
}

//~ ak: Draw functions
//=============================================================================

internal void render_draw_rect_push(Arena *arena, Render_Draw_List *list, Vec4_F32 dst, Vec4_F32 color)
{
    Render_Draw_Node *node = arena_push(arena, Render_Draw_Node, 1);
    node->type = Render_Draw_Type_Rect;
    Render_Draw_Rect *rect = &node->param_rect;
    rect->dst = dst;
    rect->color = color;
    rect->texture = render_handle_zero();
    SLLQueuePush(list->first, list->last, node);
}

internal void render_draw_rect_text_push(Arena *arena, Render_Draw_List *list, Font *font, Str8 text, Vec4_F32 dst, Vec4_F32 padding, Vec4_F32 color, Vec4_F32 font_color)
{
    render_draw_rect_push(arena, list, dst, color);
    Vec2_F32 font_pos = (Vec2_F32){ dst.x+padding.z, dst.y+padding.x };
    font_quad_push(font, text, font_pos, font_color, list, arena);
}
