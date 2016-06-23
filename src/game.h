#ifndef __GAME_H__
#define __GAME_H__

#include <stdlib.h>
#include <stdbool.h>
#include "input.h"

#define SIZEX 80
#define SIZEY 20

typedef int Coordinate;

typedef struct {
    Coordinate x;
    Coordinate y;
} Position;

typedef enum {
    TILE_UNKNOWN = 0,
    TILE_WALL,
    TILE_WALL_DARK,
    TILE_EMPTY,
    TILE_EMPTY_DARK
} Tile;

typedef Tile Map[SIZEY][SIZEX];
typedef bool Seen[SIZEY][SIZEX];

typedef struct {
    char     val;
    Position pos;
    bool     captured;
} Letter;

typedef struct {
    Position pos;
    int      hp;
} Player;

typedef struct {
    Player  player;
    char   *magic_word;
    size_t  num_letters;
    Letter *letters;
    Map     map;
    Seen    seen;
} GameState;

void game_process(GameState *, InputState *);

void game_init(GameState *, char *);

#endif
