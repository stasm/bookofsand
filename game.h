#ifndef __GAME_H__
#define __GAME_H__

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

typedef struct {
    Tile tiles[SIZEY][SIZEX];
} Map;

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

void game_init(GameState *);

#endif
