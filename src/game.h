#ifndef __GAME_H__
#define __GAME_H__

#include "input.h"

#define SIZEX 40
#define SIZEY 20

typedef int Coordinate;

typedef struct {
    Coordinate x;
    Coordinate y;
} Position;

typedef enum {
    TILE_UNKNOWN = 0,
    TILE_WALL,
    TILE_EMPTY
} Tile;

typedef Tile Map[SIZEY][SIZEX];

typedef struct {
    Position pos;
    int      hp;
} Letter;

typedef struct {
    Position pos;
    int      hp;
} Player;

typedef struct {
    Player player;
    Letter letters[10];
    Map    map;
} GameState;

void game_process(GameState *, InputState *);

void game_init(GameState *);

#endif
