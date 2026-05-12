//- GENERATED CODE

#ifndef UI_META_H
#define UI_META_H

typedef struct UI_Parent_Node UI_Parent_Node; struct UI_Parent_Node{UI_Parent_Node *next; UI_Box * v;};
typedef struct UI_SeedKey_Node UI_SeedKey_Node; struct UI_SeedKey_Node{UI_SeedKey_Node *next; UI_Key v;};
typedef struct UI_Flags_Node UI_Flags_Node; struct UI_Flags_Node{UI_Flags_Node *next; UI_Box_Flags v;};
typedef struct UI_Child_Layout_Axis_Node UI_Child_Layout_Axis_Node; struct UI_Child_Layout_Axis_Node{UI_Child_Layout_Axis_Node *next; Axis_2d v;};
typedef struct UI_Fixed_X_Node UI_Fixed_X_Node; struct UI_Fixed_X_Node{UI_Fixed_X_Node *next; float v;};
typedef struct UI_Fixed_Y_Node UI_Fixed_Y_Node; struct UI_Fixed_Y_Node{UI_Fixed_Y_Node *next; float v;};
typedef struct UI_Pref_Width_Node UI_Pref_Width_Node; struct UI_Pref_Width_Node{UI_Pref_Width_Node *next; UI_Size v;};
typedef struct UI_Pref_Height_Node UI_Pref_Height_Node; struct UI_Pref_Height_Node{UI_Pref_Height_Node *next; UI_Size v;};
typedef struct UI_Font_Size_Node UI_Font_Size_Node; struct UI_Font_Size_Node{UI_Font_Size_Node *next; float v;};
typedef struct UI_Background_Color_Node UI_Background_Color_Node; struct UI_Background_Color_Node{UI_Background_Color_Node *next; Vec4_F32 v;};
typedef struct UI_Border_Color_Node UI_Border_Color_Node; struct UI_Border_Color_Node{UI_Border_Color_Node *next; Vec4_F32 v;};
typedef struct UI_Border_Size_Node UI_Border_Size_Node; struct UI_Border_Size_Node{UI_Border_Size_Node *next; float v;};
typedef struct UI_Text_Color_Node UI_Text_Color_Node; struct UI_Text_Color_Node{UI_Text_Color_Node *next; Vec4_F32 v;};
typedef struct UI_Text_Padding_Node UI_Text_Padding_Node; struct UI_Text_Padding_Node{UI_Text_Padding_Node *next; float v;};
#define UI_DeclStackNils \
struct\
{\
UI_Parent_Node parent_nil_stack_top;\
UI_SeedKey_Node seed_key_nil_stack_top;\
UI_Flags_Node flags_nil_stack_top;\
UI_Child_Layout_Axis_Node child_layout_axis_nil_stack_top;\
UI_Fixed_X_Node fixed_x_nil_stack_top;\
UI_Fixed_Y_Node fixed_y_nil_stack_top;\
UI_Pref_Width_Node pref_width_nil_stack_top;\
UI_Pref_Height_Node pref_height_nil_stack_top;\
UI_Font_Size_Node font_size_nil_stack_top;\
UI_Background_Color_Node background_color_nil_stack_top;\
UI_Border_Color_Node border_color_nil_stack_top;\
UI_Border_Size_Node border_size_nil_stack_top;\
UI_Text_Color_Node text_color_nil_stack_top;\
UI_Text_Padding_Node text_padding_nil_stack_top;\
}
#define UI_InitStackNils(state) \
state->parent_nil_stack_top.v = &ui_box_nil;\
state->seed_key_nil_stack_top.v = ui_key_zero();\
state->flags_nil_stack_top.v = 0;\
state->child_layout_axis_nil_stack_top.v = Axis_2d_X;\
state->fixed_x_nil_stack_top.v = 0;\
state->fixed_y_nil_stack_top.v = 0;\
state->pref_width_nil_stack_top.v = ui_px(200.f, 1.f);\
state->pref_height_nil_stack_top.v = ui_px(2.f, 1.f);\
state->font_size_nil_stack_top.v = 16.f;\
state->background_color_nil_stack_top.v = (Vec4_F32){ 0.3f, 0.3f, 0.3f, 0.8f };\
state->border_color_nil_stack_top.v = (Vec4_F32){ 1, 1, 1, 0.3f };\
state->border_size_nil_stack_top.v = 1;\
state->text_color_nil_stack_top.v = (Vec4_F32){ 1, 1, 1, 1 };\
state->text_padding_nil_stack_top.v = 0.f;\

#define UI_DeclStacks \
struct\
{\
struct { UI_Parent_Node *top; UI_Box * bottom_val; UI_Parent_Node *free; uint64_t gen; bool auto_pop; } parent_stack;\
struct { UI_SeedKey_Node *top; UI_Key bottom_val; UI_SeedKey_Node *free; uint64_t gen; bool auto_pop; } seed_key_stack;\
struct { UI_Flags_Node *top; UI_Box_Flags bottom_val; UI_Flags_Node *free; uint64_t gen; bool auto_pop; } flags_stack;\
struct { UI_Child_Layout_Axis_Node *top; Axis_2d bottom_val; UI_Child_Layout_Axis_Node *free; uint64_t gen; bool auto_pop; } child_layout_axis_stack;\
struct { UI_Fixed_X_Node *top; float bottom_val; UI_Fixed_X_Node *free; uint64_t gen; bool auto_pop; } fixed_x_stack;\
struct { UI_Fixed_Y_Node *top; float bottom_val; UI_Fixed_Y_Node *free; uint64_t gen; bool auto_pop; } fixed_y_stack;\
struct { UI_Pref_Width_Node *top; UI_Size bottom_val; UI_Pref_Width_Node *free; uint64_t gen; bool auto_pop; } pref_width_stack;\
struct { UI_Pref_Height_Node *top; UI_Size bottom_val; UI_Pref_Height_Node *free; uint64_t gen; bool auto_pop; } pref_height_stack;\
struct { UI_Font_Size_Node *top; float bottom_val; UI_Font_Size_Node *free; uint64_t gen; bool auto_pop; } font_size_stack;\
struct { UI_Background_Color_Node *top; Vec4_F32 bottom_val; UI_Background_Color_Node *free; uint64_t gen; bool auto_pop; } background_color_stack;\
struct { UI_Border_Color_Node *top; Vec4_F32 bottom_val; UI_Border_Color_Node *free; uint64_t gen; bool auto_pop; } border_color_stack;\
struct { UI_Border_Size_Node *top; float bottom_val; UI_Border_Size_Node *free; uint64_t gen; bool auto_pop; } border_size_stack;\
struct { UI_Text_Color_Node *top; Vec4_F32 bottom_val; UI_Text_Color_Node *free; uint64_t gen; bool auto_pop; } text_color_stack;\
struct { UI_Text_Padding_Node *top; float bottom_val; UI_Text_Padding_Node *free; uint64_t gen; bool auto_pop; } text_padding_stack;\
}
#define UI_InitStacks(state) \
state->parent_stack.top = &state->parent_nil_stack_top; state->parent_stack.bottom_val = &ui_box_nil; state->parent_stack.free = 0; state->parent_stack.auto_pop = 0;\
state->seed_key_stack.top = &state->seed_key_nil_stack_top; state->seed_key_stack.bottom_val = ui_key_zero(); state->seed_key_stack.free = 0; state->seed_key_stack.auto_pop = 0;\
state->flags_stack.top = &state->flags_nil_stack_top; state->flags_stack.bottom_val = 0; state->flags_stack.free = 0; state->flags_stack.auto_pop = 0;\
state->child_layout_axis_stack.top = &state->child_layout_axis_nil_stack_top; state->child_layout_axis_stack.bottom_val = Axis_2d_X; state->child_layout_axis_stack.free = 0; state->child_layout_axis_stack.auto_pop = 0;\
state->fixed_x_stack.top = &state->fixed_x_nil_stack_top; state->fixed_x_stack.bottom_val = 0; state->fixed_x_stack.free = 0; state->fixed_x_stack.auto_pop = 0;\
state->fixed_y_stack.top = &state->fixed_y_nil_stack_top; state->fixed_y_stack.bottom_val = 0; state->fixed_y_stack.free = 0; state->fixed_y_stack.auto_pop = 0;\
state->pref_width_stack.top = &state->pref_width_nil_stack_top; state->pref_width_stack.bottom_val = ui_px(200.f, 1.f); state->pref_width_stack.free = 0; state->pref_width_stack.auto_pop = 0;\
state->pref_height_stack.top = &state->pref_height_nil_stack_top; state->pref_height_stack.bottom_val = ui_px(2.f, 1.f); state->pref_height_stack.free = 0; state->pref_height_stack.auto_pop = 0;\
state->font_size_stack.top = &state->font_size_nil_stack_top; state->font_size_stack.bottom_val = 16.f; state->font_size_stack.free = 0; state->font_size_stack.auto_pop = 0;\
state->background_color_stack.top = &state->background_color_nil_stack_top; state->background_color_stack.bottom_val = (Vec4_F32){ 0.3f, 0.3f, 0.3f, 0.8f }; state->background_color_stack.free = 0; state->background_color_stack.auto_pop = 0;\
state->border_color_stack.top = &state->border_color_nil_stack_top; state->border_color_stack.bottom_val = (Vec4_F32){ 1, 1, 1, 0.3f }; state->border_color_stack.free = 0; state->border_color_stack.auto_pop = 0;\
state->border_size_stack.top = &state->border_size_nil_stack_top; state->border_size_stack.bottom_val = 1; state->border_size_stack.free = 0; state->border_size_stack.auto_pop = 0;\
state->text_color_stack.top = &state->text_color_nil_stack_top; state->text_color_stack.bottom_val = (Vec4_F32){ 1, 1, 1, 1 }; state->text_color_stack.free = 0; state->text_color_stack.auto_pop = 0;\
state->text_padding_stack.top = &state->text_padding_nil_stack_top; state->text_padding_stack.bottom_val = 0.f; state->text_padding_stack.free = 0; state->text_padding_stack.auto_pop = 0;\

#define UI_AutoPopStacks(state) \
if(state->parent_stack.auto_pop) { ui_parent_pop(); state->parent_stack.auto_pop = 0; }\
if(state->seed_key_stack.auto_pop) { ui_seed_key_pop(); state->seed_key_stack.auto_pop = 0; }\
if(state->flags_stack.auto_pop) { ui_flags_pop(); state->flags_stack.auto_pop = 0; }\
if(state->child_layout_axis_stack.auto_pop) { ui_child_layout_axis_pop(); state->child_layout_axis_stack.auto_pop = 0; }\
if(state->fixed_x_stack.auto_pop) { ui_fixed_x_pop(); state->fixed_x_stack.auto_pop = 0; }\
if(state->fixed_y_stack.auto_pop) { ui_fixed_y_pop(); state->fixed_y_stack.auto_pop = 0; }\
if(state->pref_width_stack.auto_pop) { ui_pref_width_pop(); state->pref_width_stack.auto_pop = 0; }\
if(state->pref_height_stack.auto_pop) { ui_pref_height_pop(); state->pref_height_stack.auto_pop = 0; }\
if(state->font_size_stack.auto_pop) { ui_font_size_pop(); state->font_size_stack.auto_pop = 0; }\
if(state->background_color_stack.auto_pop) { ui_background_color_pop(); state->background_color_stack.auto_pop = 0; }\
if(state->border_color_stack.auto_pop) { ui_border_color_pop(); state->border_color_stack.auto_pop = 0; }\
if(state->border_size_stack.auto_pop) { ui_border_size_pop(); state->border_size_stack.auto_pop = 0; }\
if(state->text_color_stack.auto_pop) { ui_text_color_pop(); state->text_color_stack.auto_pop = 0; }\
if(state->text_padding_stack.auto_pop) { ui_text_padding_pop(); state->text_padding_stack.auto_pop = 0; }\

internal UI_Box * ui_parent_top(void);
internal UI_Key ui_seed_key_top(void);
internal UI_Box_Flags ui_flags_top(void);
internal Axis_2d ui_child_layout_axis_top(void);
internal float ui_fixed_x_top(void);
internal float ui_fixed_y_top(void);
internal UI_Size ui_pref_width_top(void);
internal UI_Size ui_pref_height_top(void);
internal float ui_font_size_top(void);
internal Vec4_F32 ui_background_color_top(void);
internal Vec4_F32 ui_border_color_top(void);
internal float ui_border_size_top(void);
internal Vec4_F32 ui_text_color_top(void);
internal float ui_text_padding_top(void);
internal UI_Box * ui_parent_bottom(void);
internal UI_Key ui_seed_key_bottom(void);
internal UI_Box_Flags ui_flags_bottom(void);
internal Axis_2d ui_child_layout_axis_bottom(void);
internal float ui_fixed_x_bottom(void);
internal float ui_fixed_y_bottom(void);
internal UI_Size ui_pref_width_bottom(void);
internal UI_Size ui_pref_height_bottom(void);
internal float ui_font_size_bottom(void);
internal Vec4_F32 ui_background_color_bottom(void);
internal Vec4_F32 ui_border_color_bottom(void);
internal float ui_border_size_bottom(void);
internal Vec4_F32 ui_text_color_bottom(void);
internal float ui_text_padding_bottom(void);
internal UI_Box * ui_parent_push(UI_Box * v);
internal UI_Key ui_seed_key_push(UI_Key v);
internal UI_Box_Flags ui_flags_push(UI_Box_Flags v);
internal Axis_2d ui_child_layout_axis_push(Axis_2d v);
internal float ui_fixed_x_push(float v);
internal float ui_fixed_y_push(float v);
internal UI_Size ui_pref_width_push(UI_Size v);
internal UI_Size ui_pref_height_push(UI_Size v);
internal float ui_font_size_push(float v);
internal Vec4_F32 ui_background_color_push(Vec4_F32 v);
internal Vec4_F32 ui_border_color_push(Vec4_F32 v);
internal float ui_border_size_push(float v);
internal Vec4_F32 ui_text_color_push(Vec4_F32 v);
internal float ui_text_padding_push(float v);
internal UI_Box * ui_parent_pop(void);
internal UI_Key ui_seed_key_pop(void);
internal UI_Box_Flags ui_flags_pop(void);
internal Axis_2d ui_child_layout_axis_pop(void);
internal float ui_fixed_x_pop(void);
internal float ui_fixed_y_pop(void);
internal UI_Size ui_pref_width_pop(void);
internal UI_Size ui_pref_height_pop(void);
internal float ui_font_size_pop(void);
internal Vec4_F32 ui_background_color_pop(void);
internal Vec4_F32 ui_border_color_pop(void);
internal float ui_border_size_pop(void);
internal Vec4_F32 ui_text_color_pop(void);
internal float ui_text_padding_pop(void);
internal UI_Box * ui_parent_next_set(UI_Box * v);
internal UI_Key ui_seed_key_next_set(UI_Key v);
internal UI_Box_Flags ui_flags_next_set(UI_Box_Flags v);
internal Axis_2d ui_child_layout_axis_next_set(Axis_2d v);
internal float ui_fixed_x_next_set(float v);
internal float ui_fixed_y_next_set(float v);
internal UI_Size ui_pref_width_next_set(UI_Size v);
internal UI_Size ui_pref_height_next_set(UI_Size v);
internal float ui_font_size_next_set(float v);
internal Vec4_F32 ui_background_color_next_set(Vec4_F32 v);
internal Vec4_F32 ui_border_color_next_set(Vec4_F32 v);
internal float ui_border_size_next_set(float v);
internal Vec4_F32 ui_text_color_next_set(Vec4_F32 v);
internal float ui_text_padding_next_set(float v);
#endif // UI_META_H
