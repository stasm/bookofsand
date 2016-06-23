#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#ifdef __APPLE__
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#endif

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
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    bkgd(COLOR_PAIR(1));
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
        case TILE_WALL_DARK:
            setcchar(pic, DARK_SHADE, WA_NORMAL, COLOR_PAIR(1), NULL);
            break;
        case TILE_EMPTY:
            setcchar(pic, MEDIUM_SHADE, WA_NORMAL, COLOR_PAIR(1), NULL);
            break;
        case TILE_EMPTY_DARK:
            setcchar(pic, LIGHT_SHADE, WA_NORMAL, COLOR_PAIR(1), NULL);
            break;
        case TILE_UNKNOWN:
        default:
            setcchar(pic, EMPTY_BLOCK, WA_NORMAL, COLOR_PAIR(1), NULL);
    }
}

/* See http://www.redblobgames.com/grids/line-drawing.html */
static bool is_visible(GameState *game, int x, int y)
{
    int dx = abs(game->player.pos.x - x);
    int dy = abs(game->player.pos.y - y);
    int steps = max(dx, dy);
    int step = 0;

    while (++step <= steps - 1) {
        double t = (double) step / (double) steps;
        int u = game->player.pos.x + t * (x - game->player.pos.x);
        int v = game->player.pos.y + t * (y - game->player.pos.y);

        game->seen[v][u] = true;

        if (game->map[v][u] == TILE_WALL)
            return false;
    }

    game->seen[y][x] = true;
    return true;
}

static bool is_near(Position pos, int x, int y)
{
    int r = 5;
    int dx = abs(pos.x - x);
    int dy = abs(pos.y - y);
    return dx * dx + dy * dy < r * r;
}

static bool in_bounds(int x, int y) {
    return x >= 0 && x < SIZEX && y >= 0 && y < SIZEY;
}

static Tile get_tile_dark(Tile tile)
{
    switch (tile) {
        case TILE_WALL:
            return TILE_WALL_DARK;
        case TILE_EMPTY:
            return TILE_EMPTY_DARK;
        default:
            return tile;
    }
}

static Tile get_tile(GameState *game, int x, int y)
{
     if (!in_bounds(x, y))
         return TILE_UNKNOWN;

     if (is_visible(game, x, y)) {
         if (is_near(game->player.pos, x, y)) {
             return game->map[y][x];
         } else {
             return get_tile_dark(game->map[y][x]);
         }
     }

     if (game->seen[y][x])
         return get_tile_dark(game->map[y][x]);

     return TILE_UNKNOWN;
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

    mvaddch(game->player.pos.y, game->player.pos.x, '@');

    refresh();
}
