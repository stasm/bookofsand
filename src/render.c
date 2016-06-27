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
    TERM_COLOR_CYAN          = 50,
    TERM_COLOR_MAGENTA       = 162,
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

    /* Letters */
    init_pair(5, TERM_COLOR_WHITE, TERM_COLOR_MEDIUM_GRAY);
    init_pair(6, 248, TERM_COLOR_MEDIUM_GRAY);
    init_pair(7, 244, TERM_COLOR_MEDIUM_GRAY);
    init_pair(8, 240, TERM_COLOR_MEDIUM_GRAY);

    /* Debug */
    init_pair(100, TERM_COLOR_MAGENTA, TERM_COLOR_CYAN);

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
        case TILE_FLOOR:
            setcchar(pic, ALPHA_0, WA_NORMAL, 3, NULL);
            break;
        case TILE_FLOOR_DARK:
            setcchar(pic, ALPHA_0, WA_NORMAL, 4, NULL);
            break;
        case TILE_CORRIDOR:
        case TILE_CORRIDOR_DARK:
            setcchar(pic, ALPHA_100, WA_NORMAL, 100, NULL);
            break;
        case TILE_UNKNOWN:
        default:
            setcchar(pic, ALPHA_0, WA_NORMAL, 0, NULL);
    }
}

/* See http://members.chello.at/~easyfilter/bresenham.html */
static bool is_visible(struct game_state *game, int x1, int y1)
{
    if (game->cheats & CHEAT_REVEAL_MAP)
        return true;

    int x0 = game->player.pos.x;
    int y0 = game->player.pos.y;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx = sgn(dx);
    int sy = sgn(dy);
    dx =  abs(dx);
    dy = -abs(dy);
    int err = dx + dy;
    int err_next;

    for(;;) {
        game->seen[y0][x0] = true;

        if (x0 == x1 && y0 == y1)
            break;

        if (game->map[y0][x0] == TILE_WALL)
            return false;

        err_next = err * 2;
        if (err_next >= dy) {
            err += dy;
            x0 += sx;
        }
        if (err_next <= dx) {
            err += dx;
            y0 += sy;
        }
    }

    return true;
}

static bool is_seen(struct game_state *game, int x, int y)
{
    return game->seen[y][x];
}

static double get_distance(struct game_state *game, int x, int y)
{
    int dx = abs(game->player.pos.x - x);
    int dy = abs(game->player.pos.y - y);
    return sqrt(dx * dx + dy * dy);
}

static int get_color_for_distance(int dist)
{
    if (dist < 6)
        return 5;
    if (dist < 11)
        return 6;
    if (dist < 16)
        return 7;

    return 8;
}

static bool in_map_bounds(int x, int y) {
    return x >= 0 && x < SIZEX && y >= 0 && y < SIZEY;
}

static enum grid_tile
get_tile_visible(struct game_state *game, enum grid_tile tile)
{
    if (game->cheats & CHEAT_COLOR_CORRIDORS)
        return tile;

    switch (tile) {
        case TILE_CORRIDOR:
            return TILE_FLOOR;
        case TILE_CORRIDOR_DARK:
            return TILE_FLOOR_DARK;
        default:
            return tile;
    }
}

static enum grid_tile get_tile_dark(enum grid_tile tile)
{
    switch (tile) {
        case TILE_WALL:
            return TILE_WALL_DARK;
        case TILE_FLOOR:
        case TILE_CORRIDOR:
            return TILE_FLOOR_DARK;
        default:
            return tile;
    }
}

static enum grid_tile get_tile(struct game_state *game, int x, int y)
{
     if (!in_map_bounds(x, y))
         return TILE_UNKNOWN;

     if (is_visible(game, x, y))
         return get_tile_visible(game, game->map[y][x]);

     if (is_seen(game, x, y))
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
        mbstowcs(val, &letter->val, 1);
        int color = get_color_for_distance(
                get_distance(game, letter->pos.x, letter->pos.y));
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

    mvprintw(SIZEY + 1, 1, "word: ");

    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].captured) {
            mvaddch(SIZEY + 1, i + 8, game->letters[i].val);
        } else {
            mvaddch(SIZEY + 1, i + 8, '_');
            render_letter(game, &game->letters[i]);
        }

    mvprintw(SIZEY + 1, game->num_letters + 12, "move: hjkl");

    render_player(game);

    refresh();
}
