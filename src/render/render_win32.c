internal Draw_Buffer render_init(Alloc alloc)
{
    // Initialize States ======================================================
    Draw_Buffer draw_buffer     = ZERO_STRUCT;
    draw_buffer.width           = wl_get_display_width();
    draw_buffer.height          = wl_get_display_height();
    draw_buffer.bytes_per_pixel = 4;
    draw_buffer.pitch           = draw_buffer.width * draw_buffer.bytes_per_pixel;
    U64 memory_size             = (draw_buffer.width * draw_buffer.height) * draw_buffer.bytes_per_pixel;
    draw_buffer.memory          = alloc_make(alloc, U8, memory_size);

    render_win32_state.memory = draw_buffer.memory;
    render_win32_state.bitmap_info.bmiHeader.biSize = sizeof(render_win32_state.bitmap_info.bmiHeader);
    render_win32_state.bitmap_info.bmiHeader.biPlanes = 1;
    render_win32_state.bitmap_info.bmiHeader.biBitCount = 32;
    render_win32_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
    render_win32_state.bitmap_info.bmiHeader.biWidth = wl_get_display_width();
    render_win32_state.bitmap_info.bmiHeader.biHeight = -wl_get_display_height();
    return draw_buffer;
}

internal void render_deinit(void)
{
}

internal void render_begin(void)
{
    render_win32_state.hdc = BeginPaint(wl_w32_state.handle, &render_win32_state.paint);
}

internal void render_end(void)
{
    StretchDIBits(
        render_win32_state.hdc, 
        0, 0, wl_get_display_width(), wl_get_display_height(),
        0, 0, wl_get_display_width(), wl_get_display_height(),
        render_win32_state.memory, &render_win32_state.bitmap_info,
        DIB_RGB_COLORS, SRCCOPY
    );
    EndPaint(wl_w32_state.handle, &render_win32_state.paint);
}
