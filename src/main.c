#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "input.h"
#include "render.h"

int main(void)
{
    GameState game;
    InputState input;

    srand(time(0));

    game_init(&game);
    render_init();

    do {
        render(&game);
        game_process(&game, &input);
    } while (!input.quit);

    render_teardown();
}
