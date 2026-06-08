//~ ak: Helper functions
//=============================================================================

internal Render_Handle render_handle_zero(void)
{
    Render_Handle handle = ZERO_STRUCT;
    return handle;
}

inline internal Render_Color render_hex_to_color(unsigned int hex)
{
    Render_Color color;
    color.r = (unsigned char)(hex >> 24) & 0xff;
    color.g = (unsigned char)(hex >> 16) & 0xff;
    color.b = (unsigned char)(hex >> 8)  & 0xff;
    color.a = (unsigned char)(hex)       & 0xff;
    return color;
}

inline internal unsigned int render_color_to_hex(Render_Color color)
{
    unsigned int hex = ((unsigned int)color.r << 24) |
                       ((unsigned int)color.g << 16) |
                       ((unsigned int)color.b << 8)  |
                       ((unsigned int)color.a);
    return hex;
}

inline internal Vec4_F32 render_color_to_vec4_f32(Render_Color color)
{
    Vec4_F32 result;
    result.x = (float)(color.r)/255.0f;
    result.y = (float)(color.g)/255.0f;
    result.z = (float)(color.b)/255.0f;
    result.w = (float)(color.a)/255.0f;
    return result;
}

internal Render_Draw_List render_draw_list_zero(void)
{
    Render_Draw_List result = ZERO_STRUCT;
    return result;
}

internal Mat4x4_F32 render_sample_channel_map_from_tex2dformat(Render_Tex2D_Format fmt)
{
  Mat4x4_F32 result =
  {
    {
      {1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1},
    }
  };
  switch(fmt)
  {
    default:{}break;
    case Render_Tex2D_Format_R8:
    {
      MemSetZeroArray(result.v[0]);
      result.v[0][0] = result.v[0][1] = result.v[0][2] = result.v[0][3] = 1.f;
    }break;
  }
  return result;
}

// ak: Core functions
//=============================================================================

internal void render_begin(void)
{
    arena_pop_to(render_state->arena, render_state->render_begin_arena_pos);
    render_state->list = render_draw_list_zero();
}

//~ ak: Draw functions
//=============================================================================

internal void render_draw_rect_push(Vec4_F32 dst, Render_Color color)
{
    Render_Draw_Node *node = arena_push(render_state->arena, Render_Draw_Node, 1);
    node->type = Render_Draw_Type_Rect;
    Render_Draw_Rect *rect = &node->param_rect;
    rect->dst = dst;
    rect->color = render_color_to_vec4_f32(color);
    rect->texture = render_handle_zero();
    SLLQueuePush(render_state->list.first, render_state->list.last, node);
}

internal void render_draw_rect_text_push(Font *font, Str8 text, Vec4_F32 dst, Vec4_F32 padding, Render_Color color, Render_Color font_color)
{
    render_draw_rect_push(dst, color);
    Vec2_F32 font_pos = (Vec2_F32){ dst.x+padding.z, dst.y+padding.x };
    font_quad_push(font, text, font_pos, font_color, &render_state->list, render_state->arena);
}
