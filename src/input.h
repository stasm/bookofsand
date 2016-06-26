#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>

enum input_dir {
    DIRECTION_NONE = 0,
    DIRECTION_NW,
    DIRECTION_N,
    DIRECTION_NE,
    DIRECTION_W,
    DIRECTION_E,
    DIRECTION_SW,
    DIRECTION_S,
    DIRECTION_SE
};

struct input_state {
    enum input_dir  dir;
    bool            quit;
    bool            torch;
    bool            cheat_reveal_map;
};

void input_init(struct input_state *);

void input_get(struct input_state *);

#endif
