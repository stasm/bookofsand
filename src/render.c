#include <ncurses.h>
#include "render.h"

void render_init(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
}

void render_teardown(void)
{
    endwin();
}

void render(GameState *game)
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
