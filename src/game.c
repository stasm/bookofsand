#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "input.h"

static struct grid_pos rand_pos(struct game_state *game)
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

bool equals(struct grid_pos a, struct grid_pos b)
{
    return a.x == b.x && a.y == b.y;
}

struct letter *get_letter(struct game_state *game, struct grid_pos pos)
{
    for (size_t i = 0; i < game->num_letters; i++)
        if (equals(game->letters[i].pos, pos))
            return &game->letters[i];

    return NULL;
}

void capture_letter(struct letter *letter)
{
    letter->captured = true;
}

void move_player(struct game_state *game, enum input_dir dir)
{
    struct grid_pos new_pos = game->player.pos;

    switch (dir) {
        case DIRECTION_NW:
        case DIRECTION_N:
        case DIRECTION_NE:
            new_pos.y--;
        default:
            break;
    }

    switch (dir) {
        case DIRECTION_NE:
        case DIRECTION_E:
        case DIRECTION_SE:
            new_pos.x++;
        default:
            break;
    }

    switch (dir) {
        case DIRECTION_SW:
        case DIRECTION_S:
        case DIRECTION_SE:
            new_pos.y++;
        default:
            break;
    }

    switch (dir) {
        case DIRECTION_NW:
        case DIRECTION_W:
        case DIRECTION_SW:
            new_pos.x--;
        default:
            break;
    }

    struct letter *letter = get_letter(game, new_pos);

    if (letter != NULL)
        capture_letter(letter);

    if (game->map[new_pos.y][new_pos.x] == TILE_EMPTY)
        game->player.pos = new_pos;
}

void game_process(struct game_state *game, struct input_state *input)
{
    input_get(input);
    move_player(game, input->dir);
}


void game_init(struct game_state *game, char *magic_word)
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

    game->player.pos = rand_pos(game);

    game->magic_word = magic_word;
    game->num_letters = strlen(magic_word);
    game->letters =
        (struct letter *) malloc(game->num_letters * sizeof(struct letter));

    if (game->letters != NULL)
        for (size_t i = 0; i < game->num_letters; i++)
            game->letters[i] = (struct letter ) {
                game->magic_word[i], rand_pos(game), false
            };
}
