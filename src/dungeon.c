#include "game.h"
#include "dungeon.h"

struct grid_pos dungeon_rand_pos(struct game_state *game)
{
    do {
        int x = rand() % SIZEX;
        int y = rand() % SIZEY;

        if (game->map[y][x] == TILE_EMPTY)
            return (struct grid_pos) { x, y };

    } while(1);
}

static void dig_room(struct game_state *game, int x, int y)
{
    int i, j;
    int c = game->map[y][x];

    if (x > 0 && x < SIZEX - 1 && y > 0 && y < SIZEY - 1 && c == TILE_WALL) {
        game->map[y][x] = TILE_EMPTY;

        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                if (rand() % 9 < 2)
                    dig_room(game, x + i - 1, y + j - 1);
    }
}

void dungeon_init(struct game_state *game)
{
    int x, y;
    int target_empty = SIZEX * SIZEY / 3;
    int current_empty;

    for (y = 0; y < SIZEY; y++)
        for (x = 0; x < SIZEX; x++) {
            game->map[y][x] = TILE_WALL;
            game->seen[y][x] = false;
        }

    do {
        dig_room(game, rand() % SIZEX, rand() % SIZEY);

        current_empty = 0;
        for (y = 0; y < SIZEY; y++) {
            for (x = 0; x < SIZEX; x++) {
                if (game->map[y][x] == TILE_EMPTY)
                    current_empty++;
            }
        }
    } while (current_empty < target_empty);

}
