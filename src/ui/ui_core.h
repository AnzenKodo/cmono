// NOTE(ak): This UI module code is reference from:
// - https://www.dgtlgrove.com/p/ui-part-2-build-it-every-frame-immediate
// - https://www.dgtlgrove.com/p/ui-part-3-the-widget-building-language
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.h
// - https://github.com/EpicGamesExt/raddebugger/blob/master/src/ui/ui_core.c

#ifndef UI_CORE_H
#define UI_CORE_H

//~ ak: Types
//=============================================================================

typedef unsigned int UI_Box_Flags;
enum
{
    UI_Box_Flag_ScrollX = (1<<0),
    UI_Box_Flag_ScrollY = (1<<1),
};

typedef enum UI_Size_Kind
{
    UI_Size_Kind_Pixels,
} UI_Size_Kind;

typedef struct UI_Size UI_Size;
struct UI_Size
{
    UI_Size_Kind kind;
    float value;
    float strictness;
};

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
    // UI_Key key;
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
    // U32 fastpath_codepoint;
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
    // U64 first_touched_build_index;
    // U64 last_touched_build_index;
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

//~ ak: Macro
//=============================================================================

#define ui_px(value, strictness)         (UI_Size){ UI_SizeKind_Pixels, value, strictness }
#define ui_em(value, strictness)         (UI_Size){ UI_SizeKind_Pixels, (value) * ui_top_font_size(), strictness }
#define ui_text_dim(padding, strictness) (UI_Size){ UI_SizeKind_TextContent, padding, strictness }
#define ui_pct(value, strictness)        (UI_Size){ UI_SizeKind_ParentPct, value, strictness }
#define ui_children_sum(strictness)      (UI_Size){ UI_SizeKind_ChildrenSum, 0.f, strictness }

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
    state->name_lower##_stack.auto_pop = false;\
    state->name_lower##_stack.gen += 1;\
    return old_value

#define UI_StackPop(state, name_upper, name_lower) \
    UI_##name_upper##_Node *popped = state->name_lower##_stack.top;\
    if(popped != &state->name_lower##_nil_stack_top)\
    {\
        SLLStackPop(state->name_lower##_stack.top);\
        SLLStackPush(state->name_lower##_stack.free, popped);\
        state->name_lower##_stack.auto_pop = false;\
        state->name_lower##_stack.gen += 1;\
    }\
    return popped->v


#define UI_StackSetNext(state, name_upper, name_lower, type, new_value) \
    UI_##name_upper##_Node *node = state->name_lower##_stack.free;\
    if(node != 0) {SLLStackPop(state->name_lower##_stack.free);}\
    else {node = arena_push(state->arena, UI_##name_upper##_Node, 1);}\
    type old_value = state->name_lower##_stack.top->v;\
    node->v = new_value;\
    SLLStackPush(state->name_lower##_stack.top, node);\
    state->name_lower##_stack.auto_pop = 1;\
    state->name_lower##_stack.gen += 1;\
    return old_value;

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

#endif // UI_CORE_H
