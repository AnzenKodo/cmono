//- GENERATED CODE

internal UI_Box * ui_parent_top(void) { UI_StackTop(ui_state, Parent, parent); }
internal Axis_2d ui_axis_top(void) { UI_StackTop(ui_state, Axis, axis); }
internal float ui_text_padding_top(void) { UI_StackTop(ui_state, Text_Padding, text_padding); }
internal Render_Color ui_background_color_top(void) { UI_StackTop(ui_state, Background_Color, background_color); }
internal Render_Color ui_text_color_top(void) { UI_StackTop(ui_state, Text_Color, text_color); }
internal Font * ui_font_top(void) { UI_StackTop(ui_state, Font, font); }
internal UI_Box * ui_parent_bottom(void) { UI_StackBottom(ui_state, Parent, parent); }
internal Axis_2d ui_axis_bottom(void) { UI_StackBottom(ui_state, Axis, axis); }
internal float ui_text_padding_bottom(void) { UI_StackBottom(ui_state, Text_Padding, text_padding); }
internal Render_Color ui_background_color_bottom(void) { UI_StackBottom(ui_state, Background_Color, background_color); }
internal Render_Color ui_text_color_bottom(void) { UI_StackBottom(ui_state, Text_Color, text_color); }
internal Font * ui_font_bottom(void) { UI_StackBottom(ui_state, Font, font); }
internal UI_Box * ui_parent_push(UI_Box * v) { UI_StackPush(ui_state, Parent, parent, UI_Box *, v); }
internal Axis_2d ui_axis_push(Axis_2d v) { UI_StackPush(ui_state, Axis, axis, Axis_2d, v); }
internal float ui_text_padding_push(float v) { UI_StackPush(ui_state, Text_Padding, text_padding, float, v); }
internal Render_Color ui_background_color_push(Render_Color v) { UI_StackPush(ui_state, Background_Color, background_color, Render_Color, v); }
internal Render_Color ui_text_color_push(Render_Color v) { UI_StackPush(ui_state, Text_Color, text_color, Render_Color, v); }
internal Font * ui_font_push(Font * v) { UI_StackPush(ui_state, Font, font, Font *, v); }
internal UI_Box * ui_parent_pop(void) { UI_StackPop(ui_state, Parent, parent); }
internal Axis_2d ui_axis_pop(void) { UI_StackPop(ui_state, Axis, axis); }
internal float ui_text_padding_pop(void) { UI_StackPop(ui_state, Text_Padding, text_padding); }
internal Render_Color ui_background_color_pop(void) { UI_StackPop(ui_state, Background_Color, background_color); }
internal Render_Color ui_text_color_pop(void) { UI_StackPop(ui_state, Text_Color, text_color); }
internal Font * ui_font_pop(void) { UI_StackPop(ui_state, Font, font); }
internal UI_Box * ui_parent_set_next(UI_Box * v) { UI_StackSetNext(ui_state, Parent, parent, UI_Box *, v); }
internal Axis_2d ui_axis_set_next(Axis_2d v) { UI_StackSetNext(ui_state, Axis, axis, Axis_2d, v); }
internal float ui_text_padding_set_next(float v) { UI_StackSetNext(ui_state, Text_Padding, text_padding, float, v); }
internal Render_Color ui_background_color_set_next(Render_Color v) { UI_StackSetNext(ui_state, Background_Color, background_color, Render_Color, v); }
internal Render_Color ui_text_color_set_next(Render_Color v) { UI_StackSetNext(ui_state, Text_Color, text_color, Render_Color, v); }
internal Font * ui_font_set_next(Font * v) { UI_StackSetNext(ui_state, Font, font, Font *, v); }
