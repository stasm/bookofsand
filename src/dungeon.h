#ifndef __DUNGEON_H__
#define __DUNGEON_H__

#include "game.h"

struct game_state;

#define SIZEX 80
#define SIZEY 20

struct grid_pos {
    int x;
    int y;
};

enum grid_tile {
    TILE_UNKNOWN = 0,
    TILE_WALL,
    TILE_WALL_DARK,
    TILE_EMPTY,
    TILE_EMPTY_DARK
};

struct grid_pos dungeon_rand_pos(struct game_state *);

void dungeon_init(struct game_state *);

#endif
