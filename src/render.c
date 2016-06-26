#define _XOPEN_SOURCE_EXTENDED

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <wchar.h>

#ifdef __APPLE__
#include <ncurses.h>
#else
#include <ncursesw/curses.h>
#endif

#include "game.h"
#include "render.h"

#define ALPHA_0   L" "
#define ALPHA_25  L"░"
#define ALPHA_50  L"▒"
#define ALPHA_75  L"▓"
#define ALPHA_100 L"█"

enum term_color {
    TERM_COLOR_BLACK         = 16,
    TERM_COLOR_ROTTEN_GREEN  = 59,
    TERM_COLOR_LIGHT_PINK    = 174,
    TERM_COLOR_LIGHT_ORANGE  = 180,
    TERM_COLOR_WHITE         = 231,
    TERM_COLOR_DARK_GRAY     = 235,
    TERM_COLOR_MEDIUM_GRAY   = 237,
    TERM_COLOR_LIGHT_GRAY    = 245,
};

void render_init(void)
{
    initscr();

    cbreak();
    noecho();
    curs_set(FALSE);

    start_color();
    init_pair(0, TERM_COLOR_WHITE, TERM_COLOR_BLACK);

    /* Walls */
    init_pair(1, TERM_COLOR_WHITE, TERM_COLOR_LIGHT_ORANGE);
    init_pair(2, TERM_COLOR_LIGHT_PINK, TERM_COLOR_ROTTEN_GREEN);

    /* Floor */
    init_pair(3, TERM_COLOR_WHITE, TERM_COLOR_MEDIUM_GRAY);
    init_pair(4, TERM_COLOR_LIGHT_GRAY, TERM_COLOR_DARK_GRAY);

    bkgd(COLOR_PAIR(0));
}

void render_teardown(void)
{
    endwin();
}

static void char_for_tile(enum grid_tile tile, cchar_t *pic)
{
    switch (tile) {
        case TILE_WALL:
            setcchar(pic, ALPHA_25, WA_NORMAL, 1, NULL);
            break;
        case TILE_WALL_DARK:
            setcchar(pic, ALPHA_25, WA_NORMAL, 2, NULL);
            break;
        case TILE_EMPTY:
            setcchar(pic, ALPHA_0, WA_NORMAL, 3, NULL);
            break;
        case TILE_EMPTY_DARK:
            setcchar(pic, ALPHA_0, WA_NORMAL, 4, NULL);
            break;
        case TILE_UNKNOWN:
        default:
            setcchar(pic, ALPHA_0, WA_NORMAL, 0, NULL);
    }
}

/* See http://www.redblobgames.com/grids/line-drawing.html */
static bool is_visible(struct game_state *game, int x, int y)
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

static bool is_illuminated(struct game_state *game, int x, int y)
{
    int r = 5;
    int dx = abs(game->player.pos.x - x);
    int dy = abs(game->player.pos.y - y);
    return dx * dx + dy * dy < r * r;
}

static bool in_map_bounds(int x, int y) {
    return x >= 0 && x < SIZEX && y >= 0 && y < SIZEY;
}

static enum grid_tile get_tile_dark(enum grid_tile tile)
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

static enum grid_tile get_tile(struct game_state *game, int x, int y)
{
     if (!in_map_bounds(x, y))
         return TILE_UNKNOWN;

     if (is_visible(game, x, y)) {
         if (is_illuminated(game, x, y)) {
             return game->map[y][x];
         } else {
             return get_tile_dark(game->map[y][x]);
         }
     }

     if (game->seen[y][x])
         return get_tile_dark(game->map[y][x]);

     return TILE_UNKNOWN;
}

void render_tile(struct game_state *game, int x, int y)
{
    cchar_t pic;
    char_for_tile(get_tile(game, x, y), &pic);
    mvadd_wch(y, x, &pic);
}

void render_letter(struct game_state *game, struct letter *letter)
{
    if (is_visible(game, letter->pos.x, letter->pos.y)) {
        wchar_t val[1];
        cchar_t pic;
        int color = is_illuminated(game, letter->pos.x, letter->pos.y) ? 3 : 4;
        mbstowcs(val, &letter->val, 1);
        setcchar(&pic, val, WA_NORMAL, color, NULL);
        mvadd_wch(letter->pos.y, letter->pos.x, &pic);
    }
}

void render_player(struct game_state *game)
{
    cchar_t pic;
    setcchar(&pic, L"@", WA_NORMAL, 3, NULL);
    mvadd_wch(game->player.pos.y, game->player.pos.x, &pic);
}


void render(struct game_state *game)
{
    int h, w;

    clear();
    getmaxyx(stdscr, h, w);

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            render_tile(game, x, y);
        }

    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].captured) {
            mvaddch(SIZEY + 1, i, game->letters[i].val);
        } else {
            mvaddch(SIZEY + 1, i, '_');
            render_letter(game, &game->letters[i]);
        }

    render_player(game);

    refresh();
}
