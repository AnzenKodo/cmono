//- GENERATED CODE

internal UI_Box *ui_parent_top(void) { UI_StackTop(ui_state, Parent, parent); }
internal UI_Box *ui_parent_bottom(void) { UI_StackBottom(ui_state, Parent, parent); }
internal UI_Box *ui_parent_push(UI_Box * v) { UI_StackPush(ui_state, Parent, parent, UI_Box *, v); }
internal UI_Box *ui_parent_pop(void) { UI_StackPop(ui_state, Parent, parent); }
internal UI_Box *ui_parent_set_next(UI_Box * v) { UI_StackSetNext(ui_state, Parent, parent, UI_Box *, v); }
