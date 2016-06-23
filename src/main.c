#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "game.h"
#include "input.h"
#include "render.h"

/* djb2 algorithm: http://www.cse.yorku.ca/~oz/hash.html */
static unsigned int hash(char *str)
{
	unsigned int hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}


int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");

    if (argc != 2) {
        printf("Say the magic word?\n");
        return 1;
    }

    struct game_state game;
    struct input_state input;

    srand(hash(argv[1]));

    game_init(&game, argv[1]);
    render_init();

    do {
        render(&game);
        game_process(&game, &input);
    } while (!input.quit);

    render_teardown();
}
