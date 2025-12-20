internal Draw_Buffer draw_init(Alloc alloc, int32_t width, int32_t height)
{
    Draw_Buffer draw_buffer     = ZERO_STRUCT;
    draw_buffer.width           = width;
    draw_buffer.height          = height;
    draw_buffer.bytes_per_pixel = 4;
    draw_buffer.pitch           = draw_buffer.width * draw_buffer.bytes_per_pixel;
    uint64_t memory_size             = (draw_buffer.width * draw_buffer.height) * draw_buffer.bytes_per_pixel;
    draw_buffer.memory          = alloc_make(alloc, uint8_t, memory_size);
    return draw_buffer;
}

internal Draw_Buffer draw_init_display(Alloc alloc)
{
    Draw_Buffer draw_buffer     = ZERO_STRUCT;
    draw_buffer.width           = wl_get_display_width();
    draw_buffer.height          = wl_get_display_height();
    draw_buffer.bytes_per_pixel = 4;
    draw_buffer.pitch           = draw_buffer.width * draw_buffer.bytes_per_pixel;
    uint64_t memory_size             = (draw_buffer.width * draw_buffer.height) * draw_buffer.bytes_per_pixel;
    draw_buffer.memory          = alloc_make(alloc, uint8_t, memory_size);
    return draw_buffer;
}

internal uint32_t draw_rgba_to_hex(const Draw_Rgba color)
{
    uint32_t result = 0;

    result = (uint32_t)(
        (color.red << 24) |
        (color.green << 16) |
        (color.blue << 8) |
        (color.alpha)
    );

    return result;
}

internal uint32_t draw_rgba_to_hex_argb(const Draw_Rgba color)
{
    uint32_t result = 0;

    result = (uint32_t)(
        (color.alpha << 24) |
        (color.red << 16) |
        (color.green << 8) |
        (color.blue)
    );

    return result;
}

internal void draw_fill(Draw_Buffer draw_buffer, Draw_Rgba color)
{
    uint32_t color_in_hex = draw_rgba_to_hex_argb(color);

    for (float y = 0; y < draw_buffer.height; y++)
    {
        for (float x = 0; x < draw_buffer.width; x++)
        {
            uint32_t *pixel = (uint32_t *)draw_buffer.memory + (uint32_t)(y * draw_buffer.width + x);
            *pixel++ = color_in_hex;
        }
    }
}

internal void draw_rect(Draw_Buffer draw_buffer, Draw_Rect rect, Draw_Rgba color)
{
    int32_t rect_x = round_f32_to_i32(rect.x);
    int32_t rect_y = round_f32_to_i32(rect.y);
    int32_t width = round_f32_to_i32(rect.width);
    int32_t height = round_f32_to_i32(rect.height);
    uint32_t color_in_hex = draw_rgba_to_hex_argb(color);

    if (width > (int32_t)draw_buffer.width) width = draw_buffer.width;
    if (height > (int32_t)draw_buffer.height) height = draw_buffer.height;
    if (rect_y < 0) rect_y = 0;
    if (rect_x < 0) rect_x = 0;

    uint8_t *row = ((uint8_t *)draw_buffer.memory) + rect_x*draw_buffer.bytes_per_pixel + rect_y*draw_buffer.pitch;

    for (int32_t y = rect_y; y < height; ++y)
    {
        uint32_t *pixel = (uint32_t *)row;
        for (int32_t x = rect_x; x < width; ++x) {
            *pixel++ = color_in_hex;
        }
        row += draw_buffer.pitch;
    }
}
