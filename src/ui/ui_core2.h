// NOTE(ak): This UI module code is reference from:
// - https://www.dgtlgrove.com/p/ui-part-2-build-it-every-frame-immediate
// - https://www.dgtlgrove.com/p/ui-part-3-the-widget-building-language
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.h
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.c

#ifndef UI_CORE_H
#define UI_CORE_H

//~ ak: Types
//=============================================================================

typedef struct UI_Box UI_Box;
struct UI_Box
{
    //- ak: persistent links
    UI_Box *hash_next;
    UI_Box *hash_prev;
    
    //- ak: per-build links/data
    UI_Box *first;
    UI_Box *last;
    UI_Box *next;
    UI_Box *prev;
    UI_Box *parent;
    size_t child_count;
    
    //- ak: per-build equipment
    UI_Key key;
    UI_Box_Flags flags;
    // UI_Key tags_key;
    Str8 string;
    // UI_TextAlign text_align;
    // Vec2F32 fixed_position;
    // Vec2F32 fixed_size;
    // Vec2F32 min_size;
    UI_Size pref_size[Axis_2d_COUNT];
    Axis_2d child_layout_axis;
    // OS_Cursor hover_cursor;
    // uint32_t fastpath_codepoint;
    // UI_Key group_key;
    // DR_Bucket *draw_bucket;
    // UI_BoxCustomDrawFunctionType *custom_draw;
    // void *custom_draw_user_data;
    Vec4_F32 background_color;
    Vec4_F32 text_color;
    Vec4_F32 border_color;
    Font *font;
    float font_size;
    // F32 tab_size;
    // FNT_RasterFlags text_raster_flags;
    // F32 corner_radii[Corner_COUNT];
    // F32 blur_size;
    // F32 transparency;
    // F32 squish;
    float text_padding;
    
    //- ak: per-build artifacts
    // DR_FStrList display_fstrs;
    // DR_FRunList display_fruns;
    Vec2_F32 rect;
    // Vec2F32 fixed_position_animated;
    // Vec2F32 position_delta;
    // FuzzyMatchRangeList fuzzy_match_ranges;
    
    //- ak: persistent data
    uint64_t first_touched_build_index;
    uint64_t last_touched_build_index;
    // U64 first_disabled_build_index;
    // F32 hot_t;
    // F32 active_t;
    // F32 disabled_t;
    // F32 focus_hot_t;
    // F32 focus_active_t;
    // F32 focus_active_disabled_t;
    // Vec2F32 view_off;
    // Vec2F32 view_off_target;
    // Vec2F32 view_bounds;
    // UI_Key default_nav_focus_hot_key;
    // UI_Key default_nav_focus_active_key;
    // UI_Key default_nav_focus_next_hot_key;
    // UI_Key default_nav_focus_next_active_key;
};

typedef uint32_t UI_Signal_Flags;
enum
{
    UI_Signal_Flag_PressedLeft         = (1<<0),
    UI_Signal_Flag_PressedMiddle       = (1<<1),
    UI_Signal_Flag_PressedRight        = (1<<2),
    UI_Signal_Flag_ReleasedLeft        = (1<<3),
    UI_Signal_Flag_ReleasedMiddle      = (1<<4),
    UI_Signal_Flag_ReleasedRight       = (1<<5),
    UI_Signal_Flag_ClickedLeft         = (1<<6),
    UI_Signal_Flag_ClickedMiddle       = (1<<7),
    UI_Signal_Flag_ClickedRight        = (1<<8),
    UI_Signal_Flag_DraggingLeft        = (1<<9),
    UI_Signal_Flag_DraggingMiddle      = (1<<10),
    UI_Signal_Flag_DraggingRight       = (1<<11),
    UI_Signal_Flag_DoubleClickedLeft   = (1<<12),
    UI_Signal_Flag_DoubleClickedMiddle = (1<<13),
    UI_Signal_Flag_DoubleClickedRight  = (1<<14),
    UI_Signal_Flag_PressedKeyboard     = (1<<15),
    UI_Signal_Flag_Hovering            = (1<<16),
    UI_Signal_Flag_MouseIsOver         = (1<<17),
};

typedef struct UI_Signal UI_Signal;
struct UI_Signal
{
    UI_Box *box;
    // OS_Modifiers modifiers;
    union
    {
        UI_Signal_Flags flags;
        struct
        {
            bool pressed_left          : 1;
            bool pressed_middle        : 1;
            bool pressed_right         : 1;
            bool released_left         : 1;
            bool released_middle       : 1;
            bool released_right        : 1;
            bool clicked_left          : 1;
            bool clicked_middle        : 1;
            bool clicked_right         : 1;
            bool dragging_left         : 1;
            bool dragging_middle       : 1;
            bool dragging_right        : 1;
            bool double_clicked_left   : 1;
            bool double_clicked_middle : 1;
            bool double_clicked_right  : 1;
            bool pressed_keyboard      : 1;
            bool hovering              : 1;
            bool mouse_is_over         : 1;
        };
    };
};

typedef struct UI_Box_Slot UI_Box_Slot;
struct UI_Box_Slot
{
    UI_Box *first;
    UI_Box *last;
};

typedef struct UI_Box_Step UI_Box_Step;
struct UI_Box_Step
{
    UI_Box *next;
    size_t push_count;
    size_t pop_count;
};

typedef struct UI_State UI_State;
struct UI_State
{
    Arena *arena;
    UI_Box *root;
    UI_Box *first_free_box;
    size_t free_box_list_count;
    size_t box_table_size;
    size_t build_index;
    UI_Box_Slot *box_table;
    UI_DeclStackNils;
    UI_DeclStacks;
};

//~ ak: Macro
//=============================================================================

#define ui_px(value, strictness)         (UI_Size){ UI_Size_Kind_Pixels, value, strictness }
#define ui_em(value, strictness)         (UI_Size){ UI_Size_Kind_Pixels, (value) * ui_font_size_top(), strictness }
#define ui_text_dim(padding, strictness) (UI_Size){ UI_Size_Kind_TextContent, padding, strictness }
#define ui_pct(value, strictness)        (UI_Size){ UI_Size_Kind_ParentPct, value, strictness }
#define ui_children_sum(strictness)      (UI_Size){ UI_Size_Kind_ChildrenSum, 0.f, strictness }

#define UI_BoxSetNil(b) ((b) = &ui_g_nil_box)
#define UI_StackTop(state, name_upper, name_lower) \
    return state->name_lower##_stack.top->v

#define UI_StackBottom(state, name_upper, name_lower) \
    return state->name_lower##_stack.bottom_val

#define UI_StackPush(state, name_upper, name_lower, type, new_value) \
    UI_##name_upper##_Node *node = state->name_lower##_stack.free;\
    if(node != 0) {SLLStackPop(state->name_lower##_stack.free);}\
    else {node = arena_push(state->arena, UI_##name_upper##_Node, 1);}\
    type old_value = state->name_lower##_stack.top->v;\
    node->v = new_value;\
    SLLStackPush(state->name_lower##_stack.top, node);\
    if(node->next == &state->name_lower##_nil_stack_top)\
    {\
        state->name_lower##_stack.bottom_val = (node->v);\
    }\
    return old_value

#define UI_StackPop(state, name_upper, name_lower) \
    UI_##name_upper##_Node *popped = state->name_lower##_stack.top;\
    if(popped != &state->name_lower##_nil_stack_top)\
    {\
        SLLStackPop(state->name_lower##_stack.top);\
        SLLStackPush(state->name_lower##_stack.free, popped);\
    }\
    return popped->v


#define UI_StackSetNext(state, name_upper, name_lower, type, new_value) \
    UI_##name_upper##_Node *node = state->name_lower##_stack.free;\
    if(node != 0) {SLLStackPop(state->name_lower##_stack.free);}\
    else {node = arena_push(state->arena, UI_##name_upper##_Node, 1);}\
    type old_value = state->name_lower##_stack.top->v;\
    node->v = new_value;\
    SLLStackPush(state->name_lower##_stack.top, node);\
    return old_value;

#define ui_box_step_reverse(box, root) ui_box_step_next(box, root, OffsetOf(UI_Box, prev), OffsetOf(UI_Box, last))

//~ ak: Functions
//=============================================================================

//~ ak: Globals
//=============================================================================

PragmaNoWarnMissingFieldInitPush()

read_only global UI_Box ui_box_nil =
{
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
    &ui_box_nil,
};

PragmaPop()

global UI_State *ui_state = 0;

#endif // UI_CORE_H
