#ifndef __RENDER_H__
#define __RENDER_H__

#include "game.h"

void render_init(void);

void render_teardown(void);

void render(struct game_state *);

#endif
