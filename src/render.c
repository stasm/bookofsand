#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <stdbool.h>
#include <ncursesw/curses.h>
#include "game.h"
#include "render.h"

#define EMPTY_BLOCK  L" "
#define LIGHT_SHADE  L"░"
#define MEDIUM_SHADE L"▒"
#define DARK_SHADE   L"▓"
#define FULL_BLOCK   L"█"

void render_init(void)
{
    initscr();

    cbreak();
    noecho();
    curs_set(FALSE);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
}

void render_teardown(void)
{
    endwin();
}

static void char_for_tile(Tile tile, cchar_t *pic)
{
    switch (tile) {
        case TILE_WALL:
            setcchar(pic, FULL_BLOCK, WA_NORMAL, COLOR_PAIR(1), NULL);
            break;
        case TILE_EMPTY:
            setcchar(pic, EMPTY_BLOCK, WA_NORMAL, COLOR_PAIR(1), NULL);
            break;
        case TILE_FOG:
        case TILE_UNKNOWN:
        default:
            setcchar(pic, LIGHT_SHADE, WA_NORMAL, COLOR_PAIR(1), NULL);
    }
}

static bool is_visible(Position pos, int x, int y)
{
    int r = 5;
    int dx = pos.x - x;
    int dy = pos.y - y;
    return dx * dx + dy * dy < r * r;
}

static Tile get_tile(GameState *game, int x, int y)
{
     if (is_visible(game->player.pos, x, y))
         if (x >= 0 && x < SIZEX && y >= 0 && y < SIZEY)
             return game->map[y][x];
         else
             return TILE_UNKNOWN;
     else
         return TILE_FOG;
}

void render(GameState *game)
{
    int h, w;
    int num_captured = 0;

    clear();
    getmaxyx(stdscr, h, w);

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            cchar_t pic;
            char_for_tile(get_tile(game, x, y), &pic);
            mvadd_wch(y, x, &pic);
        }

    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].captured)
            mvaddch(SIZEY + 1, num_captured++, game->letters[i].val);
        else
            mvaddch(
                    game->letters[i].pos.y,
                    game->letters[i].pos.x,
                    game->letters[i].val
                   );

    attron(COLOR_PAIR(1));
    mvaddch(game->player.pos.y, game->player.pos.x, '@');
    attroff(COLOR_PAIR(1));

    refresh();
}
