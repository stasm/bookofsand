#include <stdbool.h>
#include "game.h"
#include "dungeon.h"

static int get_inner_width(struct grid_area *area)
{
    return area->south_east.x - area->north_west.x - 1;
}

static int get_inner_height(struct grid_area *area)
{
    return area->south_east.y - area->north_west.y - 1;
}

static enum grid_split_dir get_split_dir(struct grid_area *area)
{
    double ratio = (double) get_inner_width(area) / get_inner_height(area);

    if (ratio > ROOM_MAX_WH_RATIO)
        return SPLIT_HORIZONTAL;
    else if ((1 / ratio) > ROOM_MAX_HW_RATIO)
        return SPLIT_VERTICAL;
    else
        return rand() % SPLIT_LENGTH;
}

static struct grid_area *create_area(struct grid_pos north_west,
                                     struct grid_pos south_east,
                                     int nesting)
{
    struct grid_area *area = malloc(sizeof(struct grid_area));
    area->north_west = north_west;
    area->south_east = south_east;
    area->nesting = nesting;
    area->split_dir = get_split_dir(area);
    area->first_leaf = NULL;
    area->second_leaf = NULL;
    return area;
}

static void free_area(struct grid_area *area)
{
    if (area == NULL)
        return;

    free_area(area->first_leaf);
    free_area(area->second_leaf);
    free(area);
}

/* return a random int in [low, high) range */
static int rand_between(int r, int low, int high)
{
    return low + (r % (high - low));
}

static int rand_area_x(struct grid_area *area, int r, int padding)
{
    return rand_between(r, area->north_west.x + padding + 1,
            area->south_east.x - padding);
}

static int rand_area_y(struct grid_area *area, int r, int padding)
{
    return rand_between(r, area->north_west.y + padding + 1,
            area->south_east.y - padding);
}

static bool is_area_big_enough(struct grid_area *area)
{
    /* area's inner dimensions must be sufficient to comprise two rooms */
    return get_inner_width(area) > 2 * ROOM_MIN_WIDTH
        && get_inner_height(area) > 2 * ROOM_MIN_HEIGHT;
}

static void split_area(struct grid_area *area)
{
    if (!is_area_big_enough(area))
            return;

    int nesting = area->nesting + 1;
    int r = rand();

    switch(area->split_dir) {
        case SPLIT_HORIZONTAL: {
            int x = rand_area_x(area, r, ROOM_MIN_WIDTH);
            area->first_leaf = create_area(
                    area->north_west,
                    (struct grid_pos) { x, area->south_east.y },
                    nesting);
            area->second_leaf = create_area(
                    (struct grid_pos) { x, area->north_west.y },
                    area->south_east,
                    nesting);
            break;
        }
        case SPLIT_VERTICAL: {
            int y = rand_area_y(area, r, ROOM_MIN_HEIGHT);
            area->first_leaf = create_area(
                    area->north_west,
                    (struct grid_pos) { area->south_east.x, y },
                    nesting);
            area->second_leaf = create_area(
                    (struct grid_pos) { area->north_west.x, y },
                    area->south_east,
                    nesting);
            break;
        }
    }

    if (nesting == MAX_NESTING)
        return;

    split_area(area->first_leaf);
    split_area(area->second_leaf);
}

struct grid_pos rand_area_pos(struct game_state *game, struct grid_area *area)
{
    do {
        int r = rand();
        int x = rand_area_x(area, r, 1);
        int y = rand_area_y(area, r, 1);

        if (game->map[y][x] == TILE_FLOOR || game->map[y][x] == TILE_CORRIDOR)
            return (struct grid_pos) { x, y };
    } while(1);
}

static void dig_room(struct game_state *game, struct grid_area *area)
{
    int r = rand();
    /* first and last good tile to start digging */
    int min_x1 = area->north_west.x + 1;
    int max_x1 = area->south_east.x - ROOM_MIN_WIDTH;
    int x1 = rand_between(r, min_x1, max_x1 + 1);
    int x2 = rand_between(r, x1 + ROOM_MIN_WIDTH - 1, area->south_east.x);
    int min_y1 = area->north_west.y + 1;
    int max_y1 = area->south_east.y - ROOM_MIN_HEIGHT;
    int y1 = rand_between(r, min_y1, max_y1 + 1);
    int y2 = rand_between(r, y1 + ROOM_MIN_HEIGHT - 1, area->south_east.y);

    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            game->map[y][x] = TILE_FLOOR;
}

/* See http://www.redblobgames.com/grids/line-drawing.html */
static void dig_line(struct game_state *game,
                     struct grid_pos p1,
                     struct grid_pos p2)
{
    int dx = abs(p1.x - p2.x);
    int dy = abs(p1.y - p2.y);
    int steps = max(dx, dy);
    int step = 0;

    while (++step <= steps) {
        double t = (double) step / (double) steps;
        int u = p1.x + t * (p2.x - p1.x);
        int v = p1.y + t * (p2.y - p1.y);
        if (game->map[v][u] == TILE_WALL)
            game->map[v][u] = TILE_CORRIDOR;
    }
}

static void dig_corridor(struct game_state *game,
                         struct grid_pos p1,
                         struct grid_pos p2)
{
    struct grid_pos via = rand() % 2 ?
        (struct grid_pos) { p1.x, p2.y } :
        (struct grid_pos) { p2.x, p1.y };

    dig_line(game, p1, via);
    dig_line(game, p2, via);
}

static void connect_rooms(struct game_state *game,
                          struct grid_area *a1,
                          struct grid_area *a2)
{
    struct grid_pos p1 = rand_area_pos(game, a1);
    struct grid_pos p2 = rand_area_pos(game, a2);
    dig_corridor(game, p1, p2);
}

static void dig_area(struct game_state *game, struct grid_area *area)
{
    if (area->first_leaf != NULL && area->second_leaf != NULL) {
        dig_area(game, area->first_leaf);
        dig_area(game, area->second_leaf);
        connect_rooms(game, area->first_leaf, area->second_leaf);
    } else {
        dig_room(game, area);
    }
}

void dungeon_init(struct game_state *game)
{
    int x, y;

    for (y = 0; y < SIZEY; y++)
        for (x = 0; x < SIZEX; x++) {
            game->map[y][x] = TILE_WALL;
            game->seen[y][x] = false;
        }

    struct grid_area *area = create_area(
            (struct grid_pos) { 0, 0 },
            (struct grid_pos) { SIZEX - 1, SIZEY - 1 },
            0);

    split_area(area);
    dig_area(game, area);
    free_area(area);
}

struct grid_pos dungeon_rand_pos(struct game_state *game)
{
    do {
        int x = rand() % SIZEX;
        int y = rand() % SIZEY;

        if (game->map[y][x] == TILE_FLOOR || game->map[y][x] == TILE_CORRIDOR)
            return (struct grid_pos) { x, y };
    } while(1);
}

