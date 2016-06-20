#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define SIZEX	40
#define	SIZEY	20
static char map[SIZEY][SIZEX];

#define EMPTY   '.'
#define WALL    '#'

#define MMAX	50
static int mx[MMAX];
static int my[MMAX];
static char mc[MMAX];
static int mhp[MMAX];

static void draw_region(int x1, int x2, int y1, int y2, char c)
{
    int x, y;

    for (y = y1; y < y2; y++) {
        for (x = x1; x < x2; x++) {
            map[y][x] = c;
        }
    }
}

static void addm(int x, int y, int hp, char c)
{
    int i;

    for (i = 0; i < MMAX; i++) {
        if (!mc[i]) {
            mx[i] = x;
            my[i] = y;
            mhp[i] = hp;
            mc[i] = c;
            return;
        }
    }
}

static int getm(int x, int y)
{
    int i;
    for (i = 0; i < MMAX; i++) {
        if ((mx[i] == x) && (my[i] == y))
            return i;
    }

    return -1;
}

static void rand_place(char c, int hp)
{
    do {
        int x = rand()&127;
        int y = rand()&63;

        if (map[y][x] == EMPTY) {
            map[y][x] = c;
            addm(x, y, hp, c);
            return;
        }
    } while(1);
}

static void dig_room(int x, int y)
{
    int i, j;
    int c = map[y][x];

    if (x > 0 && x < SIZEX -1 && y > 0 && y < SIZEY - 1 && c == WALL) {
        map[y][x] = EMPTY;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (rand() % 9 < 2)
                    dig_room(x + i - 1, y + j - 1);
            }
        }
    }
}

static void init_map(void)
{
    int i, x, y;
    int target_empty = SIZEX * SIZEY / 3;
    int current_empty;

    draw_region(0, SIZEX, 0, SIZEY, WALL);

    do {
        dig_room(rand() % SIZEX, rand() % SIZEY);

        current_empty = 0;
        for (y = 0; y < SIZEY; y++) {
            for (x = 0; x < SIZEX; x++) {
                if (map[y][x] == EMPTY)
                    current_empty++;
            }
        }
    } while (current_empty < target_empty);

    rand_place('@', 5);

    for (i = 0; i < 1; i++) {
        rand_place('m', 2);
    }
}

static void draw_screen(void)
{
    int x, y, dx, dy;

    clear();

    for (y = 0; y < SIZEY; y++) {
        for (x = 0; x < SIZEX; x++) {
            dx = mx[0] - x;
            dy = my[0] - y;
            if (dx * dx + dy * dy < 25 )
                /* if (-10 < dx && dx < 10 && -5 <= dy && dy < 5) */
                mvaddch(y, x, map[y][x]);
        }
    }
}

static int sgn(int x)
{
    return (x > 0) - (x < 0);
}

int main(void)
{
    int c;
    int i = 0;
    srand(time(0));
    init_map();

    initscr();
    cbreak();

    while (1) {
        int nx = mx[i], ny = my[i];
        if (!i) {
            draw_screen();
            c = 0;

            while (!(c=getch()));

            if ((c == 'h') || (c == 'b') || (c == 'y')) nx -= 1;
            if ((c == 'l') || (c == 'n') || (c == 'u')) nx += 1;
            if ((c == 'k') || (c == 'y') || (c == 'u')) ny -= 1;
            if ((c == 'j') || (c == 'b') || (c == 'n')) ny += 1;

            if (map[ny][nx] == 'm') {
                int j = getm(nx, ny);

                mhp[j]--;

                if (!mhp[j]) {
                    map[ny][nx] = EMPTY;
                    mc[j] = 0;
                }
            }
        }

        if (mc[i] == 'm') {
            nx += sgn(mx[0]-nx);
            ny += sgn(my[0]-ny);

            if (map[ny][nx] == '@') {
                mhp[0]--;

                if (!mhp[0]) {
                    endwin();
                    exit(0);
                }
            }
        }

        if (mc[i] && (map[ny][nx] == EMPTY)) {
            map[my[i]][mx[i]] = EMPTY;

            mx[i] = nx;
            my[i] = ny;

            map[ny][nx] = mc[i];
        }

        i++;

        if (i == MMAX) i = 0;
    }

    return(0);
}
