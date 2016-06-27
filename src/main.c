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

#define MIN_LETTERS  3
#define MAX_LETTERS  10

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
main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Specify the number of letters [3, 10].\n");
        exit(1);
    }

    setlocale(LC_ALL, "");

    int num = (int) strtol(argv[1], NULL, 10);
    if (errno == ERANGE || num < MIN_LETTERS || MAX_LETTERS < num) {
        printf("That's not a valid number, silly!\n");
        exit(1);
    }

    int count = 0;
    char word[11];
    char line[80];
    FILE *words = fopen("/usr/share/dict/words", "r");

    srand(time(0));

    while (fgets(line, sizeof line, words) != NULL) {
        strtok(line, "\n");
        if ((int) strlen(line) == num && rand() % ++count == 0)
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
