#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>

typedef enum {
    DIRECTION_NONE = 0,
    DIRECTION_NW,
    DIRECTION_N,
    DIRECTION_NE,
    DIRECTION_W,
    DIRECTION_E,
    DIRECTION_SW,
    DIRECTION_S,
    DIRECTION_SE
} Direction;

typedef struct {
    bool      quit;
    Direction dir;
} InputState;

void input_get(InputState *);

#endif
