#include <ncurses.h>
#include "input.h"

void input_init(struct input_state *input)
{
    input->dir  = DIRECTION_NONE;
    input->quit = false;
    input->torch = false;
}

static void input_clear(struct input_state *input)
{
    input->dir  = DIRECTION_NONE;
    input->quit = false;
}

void input_get(struct input_state *input)
{
    int c = getch();
    input_clear(input);

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
        case 't':
            input->torch = !input->torch;
            break;
    }
}
