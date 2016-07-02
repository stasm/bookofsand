#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "input.h"
#include "log.h"

char descriptions[][35] = {
    "\nThere's a tiny %c here. %s",
    "\nYou see an unassuming %c. %s",
    "\nThere's a small %c here. %s",
    "\nA menacing %c comes into view. %s",
    "\nYou see a looming %c. %s",
    "\nThere is a fearless %c here. %s",
    "\nA medium %c comes into view. %s",
    "\nYou see a looming %c. %s",
    "\nA rather dangerous %c appears. %s",
    "\nYou see a huge %c. %s",
    "\nA gigantic %c comes into view. %s",
    "\nA humongous %c comes into view. %s",
};

char feelings[][25] = {
    "You have nothing to fear.",
    "It's harmless.",
    "It's no match for you.",
    "It will be an easy fight.",
    "You reconsider.",
    "It will be a hard fight.",
    "It looks tough.",
    "Stay away from it.",
    "Perhaps some other time?",
    "It looks very scary.",
    "You tremble.",
    "It will kill you.",
};

bool
equals(struct grid_pos a, struct grid_pos b)
{
    return a.x == b.x && a.y == b.y;
}

struct letter *
get_letter(struct game_state *game, struct grid_pos p)
{
    for (size_t i = 0; i < game->num_letters; i++)
        if (equals(game->letters[i].pos, p))
            return &game->letters[i];

    return NULL;
}

void
capture_letter(struct game_state *game, struct letter *letter)
{
    game->player.strength++;
    letter->captured = true;
}

static bool
is_passable(struct game_state *game, struct grid_pos *p)
{
    enum grid_tile tile = game->map[p->y][p->x];
    return tile == TILE_FLOOR || tile == TILE_CORRIDOR;
}

void
move_player(struct game_state *game, enum input_dir dir)
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
        capture_letter(game, letter);

    if (is_passable(game, &new_pos))
        game->player.pos = new_pos;
}

static int
map_number(int n, int from_min, int from_max, int to_min, int to_max)
{
    return (to_max - to_min) * (n - from_min) / (from_max - from_min) + to_min;
}

static int
get_strength_diff(struct game_state *game, int player, int letter)
{
    return map_number(letter - player, -game->num_letters, game->num_letters,
            0, 11);
}


void
game_learn_letter(struct game_state *game, struct letter *letter)
{
    for (size_t i = 0; i < game->num_letters; i++)
        if (game->letters[i].val == letter->val)
            game->letters[i].known = true;

    char text[MAX_MESSAGE_LEN];
    int diff = get_strength_diff(game, game->player.strength,
            letter->strength);
    sprintf(text, descriptions[letter->strength], letter->val, feelings[diff]);
    append_message(&game->log, text);
}

void
game_process(struct game_state *game, struct input_state *input)
{
    input_get(input);

    if (input->cheat_reveal_map)
        game->cheats ^= CHEAT_REVEAL_MAP;

    if (input->cheat_color_corridors)
        game->cheats ^= CHEAT_COLOR_CORRIDORS;

    move_player(game, input->dir);
}


void
game_init(struct game_state *game, char *magic_word)
{
    dungeon_init(game);

    game->player = (struct player) {
        .pos   = dungeon_rand_pos(game),
    };

    game->magic_word = magic_word;
    game->num_letters = strlen(magic_word);
    game->letters = malloc(game->num_letters * sizeof(struct letter));

    if (game->letters != NULL)
        for (size_t i = 0; i < game->num_letters; i++)
            game->letters[i] = (struct letter) {
                .val = game->magic_word[i],
                .pos = dungeon_rand_pos(game),
                .captured = false,
                .known = false,
                .strength = i,
            };

    struct message *msg = create_message(NULL,
            "You leaf through the pages of The Book to find a new word.");
    game->log = (struct log) {
        .head = msg,
        .last = msg,
    };
    game->cheats = CHEAT_NONE;
}
