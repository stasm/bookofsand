#ifndef __RENDER_H__
#define __RENDER_H__

#include "game.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

void render_init(void);

void render_teardown(void);

void render(struct game_state *);

#endif
