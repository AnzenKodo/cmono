//- GENERATED CODE

#ifndef UI_META_H
#define UI_META_H

typedef struct UI_Parent_Node UI_Parent_Node; struct UI_Parent_Node{UI_Parent_Node *next; UI_Box * v;};
typedef struct UI_Axis_Node UI_Axis_Node; struct UI_Axis_Node{UI_Axis_Node *next; Axis_2d v;};
typedef struct UI_Text_Padding_Node UI_Text_Padding_Node; struct UI_Text_Padding_Node{UI_Text_Padding_Node *next; float v;};
typedef struct UI_Background_Color_Node UI_Background_Color_Node; struct UI_Background_Color_Node{UI_Background_Color_Node *next; Render_Color v;};
typedef struct UI_Text_Color_Node UI_Text_Color_Node; struct UI_Text_Color_Node{UI_Text_Color_Node *next; Render_Color v;};
typedef struct UI_Font_Node UI_Font_Node; struct UI_Font_Node{UI_Font_Node *next; Font * v;};
#define UI_DeclStackNils \
struct\
{\
UI_Parent_Node parent_nil_stack_top;\
UI_Axis_Node axis_nil_stack_top;\
UI_Text_Padding_Node text_padding_nil_stack_top;\
UI_Background_Color_Node background_color_nil_stack_top;\
UI_Text_Color_Node text_color_nil_stack_top;\
UI_Font_Node font_nil_stack_top;\
}
#define UI_InitStackNils(state) \
state->parent_nil_stack_top.v = &ui_box_nil;\
state->axis_nil_stack_top.v = Axis_2d_X;\
state->text_padding_nil_stack_top.v = 0;\
state->background_color_nil_stack_top.v = (Render_Color){ 0, 0, 0, 0};\
state->text_color_nil_stack_top.v = (Render_Color){ 0, 0, 0, 0};\
state->font_nil_stack_top.v = NULL;\

#define UI_DeclStacks \
struct\
{\
struct { UI_Parent_Node *top; UI_Box * bottom_val; UI_Parent_Node *free; uint64_t gen; bool auto_pop; } parent_stack;\
struct { UI_Axis_Node *top; Axis_2d bottom_val; UI_Axis_Node *free; uint64_t gen; bool auto_pop; } axis_stack;\
struct { UI_Text_Padding_Node *top; float bottom_val; UI_Text_Padding_Node *free; uint64_t gen; bool auto_pop; } text_padding_stack;\
struct { UI_Background_Color_Node *top; Render_Color bottom_val; UI_Background_Color_Node *free; uint64_t gen; bool auto_pop; } background_color_stack;\
struct { UI_Text_Color_Node *top; Render_Color bottom_val; UI_Text_Color_Node *free; uint64_t gen; bool auto_pop; } text_color_stack;\
struct { UI_Font_Node *top; Font * bottom_val; UI_Font_Node *free; uint64_t gen; bool auto_pop; } font_stack;\
}
#define UI_InitStacks(state) \
state->parent_stack.top = &state->parent_nil_stack_top; state->parent_stack.bottom_val = &ui_box_nil; state->parent_stack.free = 0; state->parent_stack.auto_pop = 0;\
state->axis_stack.top = &state->axis_nil_stack_top; state->axis_stack.bottom_val = Axis_2d_X; state->axis_stack.free = 0; state->axis_stack.auto_pop = 0;\
state->text_padding_stack.top = &state->text_padding_nil_stack_top; state->text_padding_stack.bottom_val = 0; state->text_padding_stack.free = 0; state->text_padding_stack.auto_pop = 0;\
state->background_color_stack.top = &state->background_color_nil_stack_top; state->background_color_stack.bottom_val = (Render_Color){ 0, 0, 0, 0}; state->background_color_stack.free = 0; state->background_color_stack.auto_pop = 0;\
state->text_color_stack.top = &state->text_color_nil_stack_top; state->text_color_stack.bottom_val = (Render_Color){ 0, 0, 0, 0}; state->text_color_stack.free = 0; state->text_color_stack.auto_pop = 0;\
state->font_stack.top = &state->font_nil_stack_top; state->font_stack.bottom_val = NULL; state->font_stack.free = 0; state->font_stack.auto_pop = 0;\

#define UI_AutoPopStacks(state) \
if(state->parent_stack.auto_pop) { ui_pop_parent(); state->parent_stack.auto_pop = 0; }\
if(state->axis_stack.auto_pop) { ui_pop_axis(); state->axis_stack.auto_pop = 0; }\
if(state->text_padding_stack.auto_pop) { ui_pop_text_padding(); state->text_padding_stack.auto_pop = 0; }\
if(state->background_color_stack.auto_pop) { ui_pop_background_color(); state->background_color_stack.auto_pop = 0; }\
if(state->text_color_stack.auto_pop) { ui_pop_text_color(); state->text_color_stack.auto_pop = 0; }\
if(state->font_stack.auto_pop) { ui_pop_font(); state->font_stack.auto_pop = 0; }\

internal UI_Box * ui_parent_top(void);
internal Axis_2d ui_axis_top(void);
internal float ui_text_padding_top(void);
internal Render_Color ui_background_color_top(void);
internal Render_Color ui_text_color_top(void);
internal Font * ui_font_top(void);
internal UI_Box * ui_parent_bottom(void);
internal Axis_2d ui_axis_bottom(void);
internal float ui_text_padding_bottom(void);
internal Render_Color ui_background_color_bottom(void);
internal Render_Color ui_text_color_bottom(void);
internal Font * ui_font_bottom(void);
internal UI_Box * ui_parent_push(UI_Box * v);
internal Axis_2d ui_axis_push(Axis_2d v);
internal float ui_text_padding_push(float v);
internal Render_Color ui_background_color_push(Render_Color v);
internal Render_Color ui_text_color_push(Render_Color v);
internal Font * ui_font_push(Font * v);
internal UI_Box * ui_parent_pop(void);
internal Axis_2d ui_axis_pop(void);
internal float ui_text_padding_pop(void);
internal Render_Color ui_background_color_pop(void);
internal Render_Color ui_text_color_pop(void);
internal Font * ui_font_pop(void);
internal UI_Box * ui_parent_set_next(UI_Box * v);
internal Axis_2d ui_axis_set_next(Axis_2d v);
internal float ui_text_padding_set_next(float v);
internal Render_Color ui_background_color_set_next(Render_Color v);
internal Render_Color ui_text_color_set_next(Render_Color v);
internal Font * ui_font_set_next(Font * v);
#endif // UI_META_H
