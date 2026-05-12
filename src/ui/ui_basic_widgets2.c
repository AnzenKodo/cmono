internal UI_Signal ui_label(Str8 string)
{
    UI_Box *box = ui_box_make_from_key(UI_Box_Flag_DrawText, ui_key_zero());
    ui_box_equip_text(box, string);
    UI_Signal result = ui_signal_from_box(box);
    return result;
}

internal UI_Signal ui_labelf(char *fmt, ...)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    va_list args;
    va_start(args, fmt);
    Str8 string = str8fv(scratch.arena, fmt, args);
    UI_Signal result = ui_label(string);
    va_end(args);
    arena_scratch_end(scratch);
    return result;
}

internal void ui_spacer(UI_Size size)
{
    UI_Box *parent = ui_parent_top();
    ui_pref_size_next_set(parent->child_layout_axis, size);
    ui_pref_size_next_set(Axis_2d_Flip(parent->child_layout_axis), ui_px(0, 0));
    UI_Box *box = ui_box_make(0, str8(""));
    Unused(box);
}

internal UI_Signal ui_button(Str8 string)
{
    // UI_SetNextHoverCursor(OS_CursorKind_Hand);
    UI_Box *box = ui_box_make(UI_Box_Flag_DrawBorder|
            UI_Box_Flag_DrawBackground|
            UI_Box_Flag_DrawText|
            UI_Box_Flag_DrawHotEffects|
            UI_Box_Flag_DrawActiveEffects|
            UI_Box_Flag_Clickable,
            string);
    UI_Signal result = ui_signal_from_box(box);
    return result;
}

internal UI_Signal ui_buttonf(char *fmt, ...)
{
    Arena_Temp scratch = arena_scratch_begin(0, 0);
    va_list args;
    va_start(args, fmt);
    Str8 string = str8fv(scratch.arena, fmt, args);
    UI_Signal result = ui_button(string);
    va_end(args);
    arena_scratch_end(scratch);
    return result;
}
