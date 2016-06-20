#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "game.h"

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
    int c;
    GameState game;

    srand(time(0));
    game_init(&game);

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    while (1) {
        Position npos = game.player.pos;

        draw_screen(&game);
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
