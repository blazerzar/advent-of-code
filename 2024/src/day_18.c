#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 16
#define DAY 18

#define SMALL_GRID_SIZE 7
#define GRID_SIZE 71

typedef struct Position {
    size_t x, y;
} Position;

typedef struct State {
    Position pos;
    size_t steps;
} State;

ptrdiff_t neighbors[4][2] = {
    {0,  -1},
    {0,  1 },
    {-1, 0 },
    {1,  0 },
};

void test();
Position *read_input(const char *filename);
size_t steps_to_exit(Position *positions, size_t bytes, size_t grid_size);
Position unreachable_byte(Position *positions, size_t grid_size);

int main(void) {
    test();

    Position *positions = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", steps_to_exit(positions, 1024, GRID_SIZE));
    Position unreachable = unreachable_byte(positions, GRID_SIZE);
    printf("%zu,%zu\n", unreachable.x, unreachable.y);
    arrfree(positions);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Position *positions = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(steps_to_exit(positions, 12, SMALL_GRID_SIZE), 22);
    Position unreachable = unreachable_byte(positions, SMALL_GRID_SIZE);
    assert_eq(unreachable.x, 6);
    assert_eq(unreachable.y, 1);
    arrfree(positions);
}

Position *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Position *positions = 0;
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        Position position;
        sscanf(buffer, "%zu,%zu", &position.x, &position.y);
        arrput(positions, position);
    }

    fclose(file);
    return positions;
}

// Creates a 2D grid and drops the bytes number of bytes in positions on it.
bool **drop_bytes(Position *positions, size_t bytes, size_t grid_size) {
    bool **grid = malloc(grid_size * sizeof(bool *));
    for (size_t i = 0; i < grid_size; ++i) {
        grid[i] = calloc(grid_size, sizeof(bool));
    }

    for (size_t i = 0; i < bytes; ++i) {
        grid[positions[i].y][positions[i].x] = true;
    }

    return grid;
}

void free_grid(size_t grid_size, bool **grid) {
    for (size_t i = 0; i < grid_size; ++i) {
        free(grid[i]);
    }
    free(grid);
}

// Returns the number of steps required to exit the grid.
size_t steps_to_exit(Position *positions, size_t bytes, size_t grid_size) {
    bool **grid = drop_bytes(positions, bytes, grid_size);
    bool **visited = drop_bytes(positions, 0, grid_size);

    State *q = 0;
    State start = {0};
    visited[start.pos.y][start.pos.x] = true;
    arrput(q, start);

    size_t steps = -1;
    while (arrlenu(q) && steps == SIZE_MAX) {
        State s = q[0];
        arrdel(q, 0);

        for (size_t i = 0; i < 4; ++i) {
            size_t x = s.pos.x + neighbors[i][0];
            size_t y = s.pos.y + neighbors[i][1];
            if (x >= grid_size || y >= grid_size || grid[y][x]
                || visited[y][x]) {
                continue;
            } else if (x == grid_size - 1 && y == grid_size - 1) {
                steps = s.steps + 1;
                break;
            }

            State next = {
                {x, y},
                s.steps + 1
            };
            arrput(q, next);
            visited[y][x] = true;
        }
    }

    arrfree(q);
    free_grid(grid_size, grid);
    free_grid(grid_size, visited);
    return steps;
}

// Returns the coordinates of the first byte that prevents the exit from being
// reachable from the starting position.
Position unreachable_byte(Position *positions, size_t grid_size) {
    size_t i = 0;
    size_t j = arrlenu(positions) - 1;

    while (i < j) {
        size_t mid = (i + j) / 2;
        size_t steps = steps_to_exit(positions, mid + 1, grid_size);
        if (steps == SIZE_MAX) {
            j = mid;
        } else {
            i = mid + 1;
        }
    }

    return positions[i];
}
