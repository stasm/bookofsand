#ifndef __RENDER_H__
#define __RENDER_H__

#ifdef __APPLE__
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#endif

struct render_ui {
    WINDOW  *map;
    WINDOW  *status;
    WINDOW  *log;
};

#include "game.h"


void render_init(struct render_ui *);

void render_teardown(struct render_ui *);

void render(struct game_state *, struct render_ui *);

#endif
