#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "input.h"

static Position rand_pos(GameState *game)
{
    do {
        int x = rand() % SIZEX;
        int y = rand() % SIZEY;

        if (game->map[y][x] == TILE_EMPTY)
            return (Position) { x, y };

    } while(1);
}

static void dig_room(GameState *game, int x, int y)
{
    int i, j;
    int c = game->map[y][x];

    if (x > 0 && x < SIZEX -1 && y > 0 && y < SIZEY - 1 && c == TILE_WALL) {
        game->map[y][x] = TILE_EMPTY;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (rand() % 9 < 2)
                    dig_room(game, x + i - 1, y + j - 1);
            }
        }
    }
}

void game_process(GameState *game, InputState *input)
{
    input_get(input);

    Position new_pos = game->player.pos;

    switch (input->dir) {
        case DIRECTION_NW:
        case DIRECTION_N:
        case DIRECTION_NE:
            new_pos.y--;
        default:
            break;
    }

    switch (input->dir) {
        case DIRECTION_NE:
        case DIRECTION_E:
        case DIRECTION_SE:
            new_pos.x++;
        default:
            break;
    }

    switch (input->dir) {
        case DIRECTION_SW:
        case DIRECTION_S:
        case DIRECTION_SE:
            new_pos.y++;
        default:
            break;
    }

    switch (input->dir) {
        case DIRECTION_NW:
        case DIRECTION_W:
        case DIRECTION_SW:
            new_pos.x--;
        default:
            break;
    }

    if (game->map[new_pos.y][new_pos.x] == TILE_EMPTY) {
        game->player.pos = new_pos;
    }
}


void game_init(GameState *game, char *magic_word)
{
    int x, y;
    int target_empty = SIZEX * SIZEY / 3;
    int current_empty;

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            game->map[y][x] = TILE_WALL;
        }
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

    game->player.hp = 5;
    game->player.pos = rand_pos(game);

    game->magic_word = magic_word;
    game->num_letters = strlen(magic_word);
    game->letters = (Letter *) malloc(game->num_letters * sizeof(Letter));

    if (game->letters != NULL) {
        for (size_t i = 0; i < game->num_letters; i++)
            game->letters[i] = (Letter) {
                game->magic_word[i], rand_pos(game)
            };
    }
}
