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
} GameState;

void game_process(GameState *, InputState *);

void game_init(GameState *, char *);

#endif
