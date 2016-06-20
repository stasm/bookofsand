#include <ncurses.h>
#include "input.h"

void input_clear(InputState *input)
{
    input->quit = false;
    input->dir  = DIRECTION_NONE;
}

void input_get(InputState *input)
{
    int c = 0;
    input_clear(input);

    while (!(c=getch()));

    switch (c) {
        case 'q':
            input->quit = true;
            break;
        case 'y':
            input->dir = DIRECTION_NW;
            break;
        case 'k':
            input->dir = DIRECTION_N;
            break;
        case 'u':
            input->dir = DIRECTION_NE;
            break;
        case 'h':
            input->dir = DIRECTION_W;
            break;
        case 'l':
            input->dir = DIRECTION_E;
            break;
        case 'b':
            input->dir = DIRECTION_SW;
            break;
        case 'j':
            input->dir = DIRECTION_S;
            break;
        case 'n':
            input->dir = DIRECTION_SE;
            break;
        default:
            input->dir = DIRECTION_NONE;
    }
}
