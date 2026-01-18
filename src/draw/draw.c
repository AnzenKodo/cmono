internal void draw_rect_push(Arena *arena, Draw_List *list, Vec4_F32 dst, Vec4_F32 color)
{
    Unused(dst);
    Draw_Node *node = arena_push(arena, Draw_Node, 1);
    node->param_rect.color = color;
    SLLQueuePush(list->first, list->last, node);
    list->length++;
}
