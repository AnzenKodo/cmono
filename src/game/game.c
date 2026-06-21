internal void game_init(void)
{
    Arena *arean = arena_alloc();
    game_state = arena_push(arean, Game_State, 1);
    // TODO(ak): init colors
}

internal void game_loop(void)
{
    for(Wl_Event *event = game_state->events.first; event != 0; event = event->next)
    {
        if (event->kind == Wl_Event_Kind_WindowResize)
        {
            // ak: calc cells size
            {
                game_state->cell_size = game_state->canvas_rect.x1/90;
            }
            
            {
                game_state->cells = (Vec2_I32)ZERO_STRUCT;
                for (int x = 0;
                        x < game_state->canvas_rect.x1;
                        x += game_state->cell_size)
                {
                    game_state->cells.x += 1;
                }
                for (int y = 0;
                        y < game_state->canvas_rect.y1;
                        y += game_state->cell_size)
                {
                    game_state->cells.y += 1;
                }
            }
        }
    }
    
    // ak: draw playground
    Rng2_F32 playground_rect = ZERO_STRUCT;
    {
        float playground_width  = (game_state->cell_size*80);
        float playground_height = (game_state->cell_size*50);
        float center_x = game_state->canvas_rect.x1/2;
        float center_y = game_state->canvas_rect.y1/2;
        float x = center_x - (playground_width/2);
        float y = center_y - (playground_height/2);
        float width = playground_width + x;
        float height = playground_height + y;
        playground_rect = rng2p(x, y, width, height);
        dr_rect(playground_rect, APP_FG_COLOR, 0.f, 0.f, 0.f);
    }
    
    // ak: draw random apple
    {
        float rand_x = rand_u32(os_now_unix()) % game_state->cells.x;
        float rand_y = rand_u32(os_now_unix()) % game_state->cells.y;
        float x = game_state->cell_size * rand_x;
        float y = game_state->cell_size * rand_y;
        float width  = game_state->cell_size + x;
        float height = game_state->cell_size + y;
        dr_rect(rng2p(x, y, width, height), RED_COLOR, 0.f, 0.f, 0.f);
    }
    
    // ak: show cell size
    {
        float x = game_state->canvas_rect.x1/2;
        float y = game_state->canvas_rect.y1/2;
        float width  = game_state->cell_size + x;
        float height = game_state->cell_size + y;
        dr_rect(rng2p(x, y, width, height), APP_FG_COLOR, 0.f, 0.f, 0.f);
    }
    draw_grid(playground_rect, game_state->cell_size, 1,
        BUILD_DEBUG ? APP_BG_COLOR : APP_BG_COLOR);
}
