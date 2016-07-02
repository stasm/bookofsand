#ifndef __GAME_H__
#define __GAME_H__

#include <stdlib.h>
#include <stdbool.h>
#include "dungeon.h"
#include "input.h"
#include "log.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define sgn(a)   (((a) > 0) - ((a) < 0))

struct grid_pos;

struct letter {
    char             val;
    struct grid_pos  pos;
    bool             captured;
    bool             known;
    int              strength;
};

struct player {
    struct grid_pos  pos;
    int              strength;
};

enum cheat_code {
    CHEAT_NONE             = 0,
    CHEAT_REVEAL_MAP       = 1,
    CHEAT_COLOR_CORRIDORS  = (1 << 1)
};

struct game_state {
    struct player     player;
    char             *magic_word;
    size_t            num_letters;
    struct letter    *letters;
    enum grid_tile    map[SIZEY][SIZEX];
    bool              seen[SIZEY][SIZEX];
    struct log        log;
    int               cheats;
};

void game_learn_letter(struct game_state *, struct letter *);

void game_process(struct game_state *, struct input_state *);

void game_init(struct game_state *, char *);

#endif
