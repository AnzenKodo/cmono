#ifndef MAIN_H
#define MAIN_H

#include "ccore/base/base_include.h"
#include "ccore/core_draw.h"
#include "ccore/platform/platform_include.h"

#include "ccore/base/base_include.c"
#include "ccore/platform/platform_include.c"

#include "ccore/core_img.h"
// #include "icon.h"

typedef struct Raw_Postion Raw_Postion;
struct Raw_Postion {
    I32 tilemap_x;
    I32 tilemap_y;
    F32 x;
    F32 y;
};

typedef struct Canonical_Postion Canonical_Postion;
struct Canonical_Postion {
    I32 tilemap_x;
    I32 tilemap_y;
    I32 tile_x;
    I32 tile_y;
    F32 tile_rel_x;
    F32 tile_rel_y;
};

typedef struct Game_State Game_State;
struct Game_State
{
    I32 player_tilemap_x;
    I32 player_tilemap_y;
    F32 player_x;
    F32 player_y;
};

typedef struct Tilemap Tilemap;
struct Tilemap
{
    U32 *tiles;
};

typedef struct World World;
struct World
{
    Tilemap *tilemaps;
    I32 tilemap_count_x;
    I32 tilemap_count_y;
    I32 count_x;
    I32 count_y;
    F32 upper_left_x;
    F32 upper_left_y;
    F32 tile_width;
    F32 tile_height;
};

#endif // MAIN_H
