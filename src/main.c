#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "game.h"
#include "input.h"

static void draw_screen(GameState *game)
{
    int x, y, dx, dy;

    clear();

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            dx = game->player.pos.x - x;
            dy = game->player.pos.y - y;
            if (dx * dx + dy * dy < 25 )
                switch (game->map.tiles[y][x]) {
                    case TILE_WALL:
                        mvaddch(y, x, '#');
                        break;
                    case TILE_EMPTY:
                        mvaddch(y, x, '.');
                        break;
                    case TILE_UNKNOWN:
                    default:
                        mvaddch(y, x, '?');
                }
        }
    }

    mvaddch(game->player.pos.y, game->player.pos.x, '@');

    refresh();
}

int main(void)
{
    GameState game;
    InputState input;

    srand(time(0));
    game_init(&game);

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    do {
        draw_screen(&game);
        game_process(&game, &input);
    } while (!input.quit);

    endwin();
    return(0);
}
