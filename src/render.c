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

void
render_init(struct render_ui *ui)
{
    initscr();

    ui->map = newwin(SIZEY, SIZEX, 0, 0);
    ui->status = newwin(1, SIZEX, SIZEY + 1, 0);
    ui->log = newwin(3, SIZEX, SIZEY + 2, 0);

    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

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

    wbkgd(ui->map, COLOR_PAIR(0));
    wbkgd(ui->status, COLOR_PAIR(0));
    wbkgd(ui->log, COLOR_PAIR(0));
}

void
render_teardown(struct render_ui *ui)
{
    delwin(ui->map);
    delwin(ui->status);
    delwin(ui->log);
    endwin();
}

static void
char_for_tile(enum grid_tile tile, cchar_t *pic)
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
static bool
is_visible(struct game_state *game, struct grid_pos p)
{
    if (game->cheats & CHEAT_REVEAL_MAP)
        return true;

    int x0 = game->player.pos.x, x1 = p.x;
    int y0 = game->player.pos.y, y1 = p.y;
    int dx = x1 - x0, sx = sgn(dx);
    int dy = y1 - y0, sy = sgn(dy);
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

static bool
is_seen(struct game_state *game, struct grid_pos p)
{
    return game->seen[p.y][p.x];
}

static double
get_distance(struct game_state *game, struct grid_pos p)
{
    int dx = abs(game->player.pos.x - p.x);
    int dy = abs(game->player.pos.y - p.y);
    return sqrt(dx * dx + dy * dy);
}

static int
get_color_for_distance(int dist)
{
    if (dist < 6)
        return 5;
    if (dist < 11)
        return 6;
    if (dist < 16)
        return 7;

    return 8;
}

static bool
in_map_bounds(struct grid_pos p) {
    return p.x >= 0 && p.x < SIZEX && p.y >= 0 && p.y < SIZEY;
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

static enum grid_tile
get_tile_dark(enum grid_tile tile)
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

static enum grid_tile
get_tile(struct game_state *game, struct grid_pos p)
{
     if (!in_map_bounds(p))
         return TILE_UNKNOWN;

     if (is_visible(game, p))
         return get_tile_visible(game, game->map[p.y][p.x]);

     if (is_seen(game, p))
         return get_tile_dark(game->map[p.y][p.x]);

     return TILE_UNKNOWN;
}

void
render_tile(struct game_state *game, WINDOW *win, struct grid_pos p)
{
    cchar_t pic;
    char_for_tile(get_tile(game, p), &pic);
    mvwadd_wch(win, p.y, p.x, &pic);
}

void
render_letter(struct game_state *game, WINDOW *win, struct letter *letter)
{
    if (is_visible(game, letter->pos)) {
        wchar_t val[1];
        cchar_t pic;
        mbstowcs(val, &letter->val, 1);
        int color = get_color_for_distance(get_distance(game, letter->pos));
        setcchar(&pic, val, WA_NORMAL, color, NULL);
        mvwadd_wch(win, letter->pos.y, letter->pos.x, &pic);
    }
}

void
render_player(struct game_state *game, WINDOW *win)
{
    cchar_t pic;
    setcchar(&pic, L"@", WA_NORMAL, 3, NULL);
    mvwadd_wch(win, game->player.pos.y, game->player.pos.x, &pic);
}

void
render_map(struct game_state *game, WINDOW *win)
{
    for (int y = 0; y < SIZEY; y++)
        for (int x = 0; x < SIZEX; x++)
            render_tile(game, win, (struct grid_pos) { x, y });

    for (size_t i = 0; i < game->num_letters; i++)
        if (!game->letters[i].captured) {
            render_letter(game, win, &game->letters[i]);
        }

    render_player(game, win);

    wnoutrefresh(win);
}

void
render_status(struct game_state *game, WINDOW *win)
{
    mvwprintw(win, 0, 0, ">");

    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].captured) {
            mvwaddch(win, 0, i + 2, game->letters[i].val);
        } else {
            mvwaddch(win, 0, i + 2, '_');
        }

    wnoutrefresh(win);
}

void
render(struct game_state *game, struct render_ui *ui)
{
    clear();

    wnoutrefresh(stdscr);
    render_map(game, ui->map);
    render_status(game, ui->status);

    doupdate();
}
