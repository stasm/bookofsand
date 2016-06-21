#include <ncurses.h>
#include <stdbool.h>
#include "game.h"
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

static char char_for_tile(Tile tile)
{
    switch (tile) {
        case TILE_WALL:
            return '#';
        case TILE_EMPTY:
            return '.';
        case TILE_UNKNOWN:
        default:
            return '?';
    }
}

static bool is_visible(Position pos, int x, int y)
{
    int dx = pos.x - x;
    int dy = pos.y - y;
    return dx * dx + dy * dy < 25;
}

void render(GameState *game)
{
    int x, y;

    clear();

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            if (is_visible(game->player.pos, x, y))
                mvaddch(y, x, char_for_tile(game->map.tiles[y][x]));
        }
    }

    mvaddch(game->player.pos.y, game->player.pos.x, '@');

    refresh();
}
