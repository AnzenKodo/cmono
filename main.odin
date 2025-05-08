package main

import rl "vendor:raylib"
import vmem "core:mem/virtual"

Tilemap :: struct {
    tiles: ^i32
}

World :: struct {
    tilemaps: ^Tilemap,
    tilemap_count_x: i32,
    tilemap_count_y: i32,
    count_x: i32,
    count_y: i32,
    upper_left_x: f32,
    upper_left_y: f32,
    tile_width: f32,
    tile_height: f32,

    tile_side_in_meters: f32,
    tile_side_in_pixels: i32,
}

Game_State :: struct {
    player_tilemap_x: i32,
    player_tilemap_y: i32,
    player_x: f32,
    player_y: f32,
};


main :: proc() {
    rl.SetTraceLogLevel(.NONE);
   	width  := i32(500)
	height := i32(350)
    rl.InitWindow(width, height, "Scuttle")
    defer rl.CloseWindow()
    rl.SetTargetFPS(60)

    arena: vmem.Arena
    arena_err := vmem.arena_init_growing(&arena)
    ensure(arena_err == nil)
    arena_alloc := vmem.arena_allocator(&arena)

    // Game Logic ==============================================================

    tilemap_count_x :: 15
    tilemap_count_y :: 10

    tiles00 : [tilemap_count_y][tilemap_count_x]i32 = {
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
        { 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    tiles01 : [tilemap_count_y][tilemap_count_x]i32 = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    tiles10 : [tilemap_count_y][tilemap_count_x]i32 = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };
    tiles11 : [tilemap_count_y][tilemap_count_x]i32 = {
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
        { 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    };

    tilemaps : [2][2]Tilemap;
    tilemaps[0][0].tiles = &tiles00[0][0];
    tilemaps[0][1].tiles = &tiles01[0][0];
    tilemaps[1][0].tiles = &tiles10[0][0];
    tilemaps[1][1].tiles = &tiles11[0][0];

    world: World
    world.count_x = tilemap_count_x;
    world.count_y = tilemap_count_y;
    world.upper_left_x = -30;
    world.upper_left_y = 0;
    world.tile_width = 30;
    world.tile_height = 30;
    world.tilemap_count_x = 2;
    world.tilemap_count_y = 2;
    world.tilemaps = &tilemaps[0][0];
    world.tile_side_in_meters = 1.4;
    world.tile_side_in_pixels = 60;
    player_width: f32 = 0.75 * world.tile_width;
    player_height: f32 = world.tile_height;

    game_state := new(Game_State, arena_alloc)
    game_state.player_x = 50
    game_state.player_y = 260

   	for !rl.WindowShouldClose() {
        rl.ClearBackground(rl.ORANGE)
        rl.BeginDrawing()
        rl.EndDrawing()
    }
}
