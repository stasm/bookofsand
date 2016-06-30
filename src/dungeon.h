#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "game.h"

struct game_state;

#define SIZEX 78
#define SIZEY 20

struct grid_pos {
    int x;
    int y;
};

enum grid_tile {
    TILE_UNKNOWN = 0,
    TILE_WALL,
    TILE_WALL_DARK,
    TILE_FLOOR,
    TILE_FLOOR_DARK,
    TILE_CORRIDOR,
    TILE_CORRIDOR_DARK
};

#define SPLIT_LENGTH  2

enum grid_split_dir {
    SPLIT_VERTICAL,
    SPLIT_HORIZONTAL
};

struct grid_area {
    struct grid_pos      north_west;
    struct grid_pos      south_east;
    int                  nesting;
    enum grid_split_dir  split_dir;
    struct grid_area    *first_leaf;
    struct grid_area    *second_leaf;
};

void dungeon_init(struct game_state *);

struct grid_pos dungeon_rand_pos(struct game_state *);

#endif
