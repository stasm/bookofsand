#include <ncursesw/curses.h>
#include "input.h"

void input_clear(struct input_state *input)
{
    input->dir  = DIRECTION_NONE;
    input->quit = false;
    input->cheat_reveal_map = false;
    input->cheat_color_corridors = false;
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
        case '7':
            input->dir = DIRECTION_NW;
            break;
        case 'k':
        case '8':
        case KEY_UP:
            input->dir = DIRECTION_N;
            break;
        case 'u':
        case '9':
            input->dir = DIRECTION_NE;
            break;
        case 'h':
        case '4':
        case KEY_LEFT:
            input->dir = DIRECTION_W;
            break;
        case 'l':
        case '6':
        case KEY_RIGHT:
            input->dir = DIRECTION_E;
            break;
        case 'b':
        case '1':
            input->dir = DIRECTION_SW;
            break;
        case 'j':
        case '2':
        case KEY_DOWN:
            input->dir = DIRECTION_S;
            break;
        case 'n':
        case '3':
            input->dir = DIRECTION_SE;
            break;
        case 'z':
            input->cheat_reveal_map = true;
            break;
        case 'x':
            input->cheat_color_corridors = true;
            break;
        default:
            input->dir = DIRECTION_NONE;
    }
}
