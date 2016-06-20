#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "game.h"

static GameState game;

static void rand_place()
{
    do {
        int x = rand() % SIZEX;
        int y = rand() % SIZEY;

        if (game.map.tiles[y][x] == TILE_EMPTY) {
            game.player.pos.x = x;
            game.player.pos.y = y;
            return;
        }
    } while(1);
}

static void dig_room(int x, int y)
{
    int i, j;
    int c = game.map.tiles[y][x];

    if (x > 0 && x < SIZEX -1 && y > 0 && y < SIZEY - 1 && c == TILE_WALL) {
        game.map.tiles[y][x] = TILE_EMPTY;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (rand() % 9 < 2)
                    dig_room(x + i - 1, y + j - 1);
            }
        }
    }
}

static void init_map(void)
{
    int x, y;
    int target_empty = SIZEX * SIZEY / 3;
    int current_empty;

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            game.map.tiles[y][x] = TILE_WALL;
        }
    }

    do {
        dig_room(rand() % SIZEX, rand() % SIZEY);

        current_empty = 0;
        for (y = 0; y < SIZEY; y++) {
            for (x = 0; x < SIZEX; x++) {
                if (game.map.tiles[y][x] == TILE_EMPTY)
                    current_empty++;
            }
        }
    } while (current_empty < target_empty);

    game.player.hp = 5;
    rand_place();
}

static void draw_screen(void)
{
    int x, y, dx, dy;

    clear();

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            dx = game.player.pos.x - x;
            dy = game.player.pos.y - y;
            if (dx * dx + dy * dy < 25 )
                switch (game.map.tiles[y][x]) {
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

    mvaddch(game.player.pos.y, game.player.pos.x, '@');

    refresh();
}

int main(void)
{
    int c;
    srand(time(0));
    init_map();

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    while (1) {
        Position npos = game.player.pos;

        draw_screen();
        c = 0;

        while (!(c=getch()));

        if ((c == 'h') || (c == 'b') || (c == 'y')) npos.x--;
        if ((c == 'l') || (c == 'n') || (c == 'u')) npos.x++;
        if ((c == 'k') || (c == 'y') || (c == 'u')) npos.y--;
        if ((c == 'j') || (c == 'b') || (c == 'n')) npos.y++;

        if (game.map.tiles[npos.y][npos.x] == TILE_EMPTY) {

            game.player.pos = npos;
        }
    }

    endwin();
    return(0);
}
