internal void draw_rect_push(Arena *arena, Draw_List *list, Vec4_F32 dst, Vec4_F32 color)
{
    Unused(dst);
    Draw_Node *node = arena_push(arena, Draw_Node, 1);
    node->type = Draw_Type_Rect;
    Draw_Rect *rect = &node->param_rect;
    rect->dst = dst;
    rect->color = color;
    SLLQueuePush(list->first, list->last, node);
}
