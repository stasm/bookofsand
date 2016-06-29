#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "game.h"
#include "input.h"
#include "render.h"

#define MIN_LETTERS  7
#define MAX_LETTERS  12

/* djb2 algorithm: http://www.cse.yorku.ca/~oz/hash.html */
static unsigned int
hash(char *str)
{
	unsigned int hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

int
main()
{
    setlocale(LC_ALL, "");

    int count = 0;
    char word[13];
    char line[80];
    FILE *words = fopen("/usr/share/dict/words", "r");

    srand(time(0));

    while (fgets(line, sizeof line, words) != NULL) {
        strtok(line, "\n");
        size_t len = strlen(line);
        if (MIN_LETTERS <= len && len <= MAX_LETTERS && rand() % ++count == 0)
            strcpy(word, line);
    }

    struct game_state game;
    struct input_state input;

    srand(hash(word));

    render_init();
    game_init(&game, word);

    do {
        render(&game);
        game_process(&game, &input);
    } while (!input.quit);


    render_teardown();

    printf("The magic word was '%s'\n", word);
}
