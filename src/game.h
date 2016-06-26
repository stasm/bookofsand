#ifndef __GAME_H__
#define __GAME_H__

#include <stdlib.h>
#include <stdbool.h>
#include "dungeon.h"
#include "input.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

struct grid_pos;

struct letter {
    char             val;
    struct grid_pos  pos;
    bool             captured;
};

struct player {
    struct grid_pos  pos;
    bool             torch;
};

struct game_state {
    struct player     player;
    char             *magic_word;
    size_t            num_letters;
    struct letter    *letters;
    struct grid_area *area;
    enum grid_tile    map[SIZEY][SIZEX];
    bool              seen[SIZEY][SIZEX];
};

void game_process(struct game_state *, struct input_state *);

void game_init(struct game_state *, char *);

#endif
