//- GENERATED CODE

#ifndef UI_META_H
#define UI_META_H

typedef struct UI_Parent_Node UI_Parent_Node; struct UI_Parent_Node{UI_Parent_Node *next; UI_Box * v;};
#define UI_DeclStackNils \
struct\
{\
UI_Parent_Node parent_nil_stack_top;\
}
#define UI_InitStackNils(state) \
state->parent_nil_stack_top.v = &ui_nil_box;\

#define UI_DeclStacks \
struct\
{\
struct { UI_Parent_Node *top; UI_Box * bottom_val; UI_Parent_Node *free; U64 gen; B32 auto_pop; } parent_stack;\
}
#define UI_InitStacks(state) \
state->parent_stack.top = &state->parent_nil_stack_top; state->parent_stack.bottom_val = &ui_nil_box; state->parent_stack.free = 0; state->parent_stack.auto_pop = 0;\

#define UI_AutoPopStacks(state) \
if(state->parent_stack.auto_pop) { ui_pop_parent(); state->parent_stack.auto_pop = 0; }\

internal UI_Box *ui_parent_top(void);
internal UI_Box *ui_parent_bottom(void);
internal UI_Box *ui_parent_push(UI_Box * v);
internal UI_Box *ui_parent_pop(void);
internal UI_Box *ui_parent_set_next(UI_Box * v);
#endif // UI_META_H
