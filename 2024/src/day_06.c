#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 256
#define DAY 06

typedef enum Direction { UP, RIGHT, DOWN, LEFT, NUM_DIRS } Direction;
typedef struct Position {
    size_t x, y;
    Direction dir;
} Position;
typedef struct Map {
    char **map;
    size_t rows;
    size_t cols;
    Position start;
} Map;

const ptrdiff_t deltas[4][2] = {
    [UP] = {0,  -1},
    [DOWN] = {0,  1 },
    [LEFT] = {-1, 0 },
    [RIGHT] = {1,  0 },
};

void test();
Map read_input(const char *filename);
void free_map(struct Map map);
size_t possible_obstacles(Map map, Position pos, bool *visited, bool obstacle);
size_t visited_positions(Map map, size_t *obstacles);

int main(void) {
    test();

    Map map = read_input("inputs/day_" STR(DAY) ".txt");
    size_t obstacles = 0;
    printf("%zu\n", visited_positions(map, &obstacles));
    printf("%zu\n", obstacles);
    free_map(map);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Map map = read_input("inputs/day_" STR(DAY) "_test.txt");
    size_t obstacles = 0;
    assert_eq(visited_positions(map, &obstacles), 41);
    assert_eq(obstacles, 6);
    free_map(map);
}

// Tries to find the starting position in the last row of the map.
void find_starting_position(Map *map) {
    const char *dirs = "^>v<";

    for (size_t i = 0; i < map->cols; ++i) {
        char c = map->map[map->rows - 1][i];
        char *start_char = strchr(dirs, c);
        if (!start_char) {
            continue;
        }

        map->start.x = i;
        map->start.y = map->rows - 1;
        map->map[map->start.y][map->start.x] = '.';
        map->start.dir = start_char - dirs;
    }
}

Map read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Map map = {0};

    while (fgets(buffer, MAX_LINE, file)) {
        char *line_end = strchr(buffer, '\n');
        if (line_end) {
            *line_end = '\0';
        }

        char *line = malloc(strlen(buffer));
        memcpy((void *) line, buffer, strlen(buffer));
        arrput(map.map, line);

        ++map.rows;
        map.cols = strlen(buffer);
        find_starting_position(&map);
    }

    fclose(file);
    return map;
}

void free_map(struct Map map) {
    for (size_t i = 0; i < map.rows; ++i) {
        free((void *) map.map[i]);
    }
    arrfree(map.map);
}

void move(size_t *x, size_t *y, Direction dir) {
    *x += deltas[dir][0];
    *y += deltas[dir][1];
}

size_t visited_index(Map map, size_t x, size_t y, Direction dir) {
    return (y * map.cols + x) * NUM_DIRS + dir;
}

// Returns true if adding an obstacle in front of the guard, at pos, causes the
// guard to get stuck in a cycle.
bool obstacle_causes_cycle(Map map, Position pos, bool *visited) {
    bool can_add = pos.x < map.cols && pos.y < map.rows
                   && map.map[pos.y][pos.x] == '.';
    for (size_t i = 0; i < NUM_DIRS; ++i) {
        can_add &= !visited[visited_index(map, pos.x, pos.y, i)];
    }

    if (!can_add) {
        return false;
    }

    size_t size = map.rows * map.cols * NUM_DIRS * sizeof(bool);
    bool *visited_copy = malloc(size);
    memcpy(visited_copy, visited, size);

    map.map[pos.y][pos.x] = '#';
    bool cycle = possible_obstacles(map, pos, visited_copy, false);
    map.map[pos.y][pos.x] = '.';

    free(visited_copy);
    return cycle;
}

// Returns the number of possible obstacles that can be placed to cause a
// cycle. The visited array is filled with visited positions of the guard
// without adding any obstacles. We only add obstacles at the first recursion
// level, where obstacle is true.
size_t possible_obstacles(Map map, Position pos, bool *visited, bool obstacle) {
    bool *obstacles = calloc(map.rows * map.cols, sizeof(bool));

    while (pos.x < map.cols && pos.y < map.rows) {
        // Move back and turn right on obstacles
        if (map.map[pos.y][pos.x] == '#') {
            move(&pos.x, &pos.y, (pos.dir + 2) % NUM_DIRS);
            pos.dir = (pos.dir + 1) % NUM_DIRS;
        }

        // If we have already visited this position, we are in a cycle.
        if (visited[visited_index(map, pos.x, pos.y, pos.dir)] && !obstacle) {
            free(obstacles);
            return 1;
        }
        visited[visited_index(map, pos.x, pos.y, pos.dir)] = true;
        move(&pos.x, &pos.y, pos.dir);

        // Try adding an obstacle at the next position
        if (obstacle && obstacle_causes_cycle(map, pos, visited)) {
            obstacles[pos.y * map.cols + pos.x] = true;
        }
    }

    size_t count = 0;
    for (size_t i = 0; i < map.rows * map.cols; ++i) {
        count += obstacles[i];
    }

    free(obstacles);
    return count;
}

// Returns the number of distinct positions visited by the guard before
// leaving the mapped area. The number of possible obstacles that cause a
// cycle is stored in the obstacles pointer.
size_t visited_positions(Map map, size_t *obstacles) {
    size_t size = map.rows * map.cols * NUM_DIRS;
    bool *visited = calloc(size, sizeof(bool));
    *obstacles = possible_obstacles(map, map.start, visited, true);

    size_t count = 0;
    for (size_t i = 0; i < size; i += NUM_DIRS) {
        bool is_visited = false;
        for (size_t k = 0; k < NUM_DIRS; ++k) {
            is_visited |= visited[i + k];
        }
        count += is_visited;
    }

    free(visited);
    return count;
}
