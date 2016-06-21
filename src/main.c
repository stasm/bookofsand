#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "input.h"
#include "render.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Say the magic word?\n");
        return 1;
    }

    GameState game;
    InputState input;

    srand(time(0));

    game_init(&game, argv[1]);
    render_init();

    do {
        render(&game);
        game_process(&game, &input);
    } while (!input.quit);

    render_teardown();
}
