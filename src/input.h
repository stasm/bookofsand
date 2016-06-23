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
    bool            quit;
    enum input_dir  dir;
};

void input_get(struct input_state *);

#endif
