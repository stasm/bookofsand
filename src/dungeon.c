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

/* return a random int in [low, high) range */
static int rand_between(int r, int low, int high)
{
    return low + (r % (high - low));
}

static int rand_area_x(struct grid_area *area, int r)
{
    return rand_between(r, area->north_west.x + ROOM_MIN_WIDTH + 1,
            area->south_east.x - ROOM_MIN_WIDTH);
}

static int rand_area_y(struct grid_area *area, int r)
{
    return rand_between(r, area->north_west.y + ROOM_MIN_HEIGHT + 1,
            area->south_east.y - ROOM_MIN_HEIGHT);
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
            int x = rand_area_x(area, r);
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
            int y = rand_area_y(area, r);
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

    area->room = (struct grid_room) {
        (struct grid_pos) { x1, y1 }, (struct grid_pos) { x2, y2 }
    };

    for (int y = y1; y <= y2; y++)
        for (int x = x1; x <= x2; x++)
            game->map[y][x] = TILE_EMPTY;
}

static bool rooms_overlap(struct grid_room a, struct grid_room b,
                            enum grid_split_dir split_dir)
{
    switch (split_dir) {
        case SPLIT_HORIZONTAL: {
            int ha = a.south_east.y - a.north_west.y + 1;
            int hb = b.south_east.y - b.north_west.y + 1;
            int diff = a.south_east.y - b.north_west.y + 1;

            return ROOM_MIN_OVERLAP <= diff
                && diff <= ha + hb - ROOM_MIN_OVERLAP;
        }
        case SPLIT_VERTICAL: {
            int wa = a.south_east.x - a.north_west.x + 1;
            int wb = b.south_east.x - b.north_west.x + 1;
            int diff = a.south_east.x - b.north_west.x +1;

            return ROOM_MIN_OVERLAP <= diff
                && diff <= wa + wb - ROOM_MIN_OVERLAP;
        }
    }

    return false;
}

static void dig_corridor(struct game_state *game, int x1, int y1, int x2,
                         int y2)
{
    for (int y = y1; y <= y2; y++)
        game->map[y][x2] = TILE_EMPTY;
    for (int x = x1; x <= x2; x++)
        game->map[y1][x] = TILE_EMPTY;
}

static void connect_rooms(struct game_state *game, struct grid_area *a,
                          struct grid_area *b, enum grid_split_dir split_dir)
{
    if (rooms_overlap(a->room, b->room, split_dir)) {
        int r = rand();
        switch (split_dir) {
            case SPLIT_HORIZONTAL: {
                int min_y = max(a->room.north_west.y, b->room.north_west.y);
                int max_y = min(a->room.south_east.y, b->room.south_east.y);
                int x1 = a->room.south_east.x;
                int x2 = b->room.north_west.x;
                int y = rand_between(r, min_y, max_y + 1);
                dig_corridor(game, x1, y, x2, y);
            }
            case SPLIT_VERTICAL: {
                int min_x = max(a->room.north_west.x, b->room.north_west.x);
                int max_x = min(a->room.south_east.x, b->room.south_east.x);
                int x = rand_between(r, min_x, max_x + 1);
                int y1 = a->room.south_east.y;
                int y2 = b->room.north_west.y;
                dig_corridor(game, x, y1, x, y2);
            }
        }
    }
}

static void dig_area(struct game_state *game, struct grid_area *area)
{
    if (area->first_leaf != NULL && area->second_leaf != NULL) {
        dig_area(game, area->first_leaf);
        dig_area(game, area->second_leaf);
        connect_rooms(game, area->first_leaf, area->second_leaf, area->split_dir);
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
            game->seen[y][x] = true;
        }

    game->area = create_area(
            (struct grid_pos) { 0, 0 },
            (struct grid_pos) { SIZEX - 1, SIZEY - 1 },
            0);

    split_area(game->area);
    dig_area(game, game->area);
}

struct grid_pos dungeon_rand_pos(struct game_state *game)
{
    do {
        int x = rand() % SIZEX;
        int y = rand() % SIZEY;

        if (game->map[y][x] == TILE_EMPTY)
            return (struct grid_pos) { x, y };

    } while(1);
}

