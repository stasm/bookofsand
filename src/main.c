#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include "game.h"
#include "input.h"

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
    GameState game;
    InputState input;

    srand(time(0));
    game_init(&game);

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    do {
        draw_screen(&game);
        input_get(&input);

        Position npos = game.player.pos;

        switch (input.dir) {
            case DIRECTION_NW:
            case DIRECTION_N:
            case DIRECTION_NE:
                npos.y--;
            default:
                break;
        }

        switch (input.dir) {
            case DIRECTION_NE:
            case DIRECTION_E:
            case DIRECTION_SE:
                npos.x++;
            default:
                break;
        }

        switch (input.dir) {
            case DIRECTION_SW:
            case DIRECTION_S:
            case DIRECTION_SE:
                npos.y++;
            default:
                break;
        }

        switch (input.dir) {
            case DIRECTION_NW:
            case DIRECTION_W:
            case DIRECTION_SW:
                npos.x--;
            default:
                break;
        }

        if (game.map.tiles[npos.y][npos.x] == TILE_EMPTY) {
            game.player.pos = npos;
        }

    } while (!input.quit);

    endwin();
    return(0);
}
