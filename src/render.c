#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
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
        case TILE_EMPTY:
            return '.';
        case TILE_WALL:
        case TILE_UNKNOWN:
        default:
            return '#';
    }
}

static bool is_visible(Position pos, int x, int y)
{
    int r = 5;
    int dx = pos.x - x;
    int dy = pos.y - y;
    return dx * dx + dy * dy < r * r;
}

void render(GameState *game)
{
    int h, w;
    int num_captured = 0;

    clear();
    getmaxyx(stdscr, h, w);

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            if (is_visible(game->player.pos, x, y))
                mvaddch(y, x, char_for_tile(game->map[y][x]));

    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].captured)
            mvaddch(SIZEY + 1, num_captured++, game->letters[i].val);
        else
            mvaddch(
                    game->letters[i].pos.y,
                    game->letters[i].pos.x,
                    game->letters[i].val
                   );

    mvaddch(game->player.pos.y, game->player.pos.x, '@');

    refresh();
}
