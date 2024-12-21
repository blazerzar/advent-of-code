#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 256
#define DAY 20

typedef struct Position {
    size_t x;
    size_t y;
} Position;

ptrdiff_t neighbors[4][2] = {
    {0,  -1},
    {1,  0 },
    {0,  1 },
    {-1, 0 },
};

void test();
char **read_input(const char *filename);
void free_racetrack(char **racetrack);
size_t good_cheats(char **racetrack, size_t duration, size_t saving);

int main(void) {
    test();

    char **racetrack = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", good_cheats(racetrack, 2, 100));
    printf("%zu\n", good_cheats(racetrack, 20, 100));
    free_racetrack(racetrack);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    char **racetrack = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(good_cheats(racetrack, 2, 64), 1);
    assert_eq(good_cheats(racetrack, 2, 40), 2);
    assert_eq(good_cheats(racetrack, 2, 20), 5);

    assert_eq(good_cheats(racetrack, 20, 76), 3);
    assert_eq(good_cheats(racetrack, 20, 74), 7);
    assert_eq(good_cheats(racetrack, 20, 72), 29);
    assert_eq(good_cheats(racetrack, 20, 70), 41);
    assert_eq(good_cheats(racetrack, 20, 68), 55);
    assert_eq(good_cheats(racetrack, 20, 66), 67);
    assert_eq(good_cheats(racetrack, 20, 64), 86);
    free_racetrack(racetrack);
}

char **read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char **racetrack = 0;
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        size_t len = strlen(buffer);
        buffer[len - 1] = '\0';
        char *line = malloc(len);
        strcpy(line, buffer);
        arrput(racetrack, line);
    }

    fclose(file);
    return racetrack;
}

void free_racetrack(char **racetrack) {
    for (size_t i = 0; i < arrlenu(racetrack); ++i) {
        free(racetrack[i]);
    }
    arrfree(racetrack);
}

size_t **init_times(size_t rows, size_t cols) {
    size_t **times = malloc(rows * sizeof(size_t *));
    for (size_t i = 0; i < rows; ++i) {
        times[i] = malloc(cols * sizeof(size_t));
        for (size_t j = 0; j < cols; ++j) {
            times[i][j] = SIZE_MAX;
        }
    }
    return times;
}

void free_times(size_t rows, size_t **times) {
    for (size_t i = 0; i < rows; ++i) {
        free(times[i]);
    }
    free(times);
}

bool **init_visited(size_t rows, size_t cols, Position start) {
    bool **visited = malloc(rows * sizeof(bool *));
    for (size_t i = 0; i < rows; ++i) {
        visited[i] = calloc(cols, sizeof(bool));
    }
    visited[start.y][start.x] = true;
    return visited;
}

void free_visited(size_t rows, bool **visited) {
    for (size_t i = 0; i < rows; ++i) {
        free(visited[i]);
    }
    free(visited);
}

// Returns the position of the first occurrence of c in racetrack.
Position find_position(size_t rows, size_t cols, char **racetrack, char c) {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (racetrack[i][j] == c) {
                return (Position){j, i};
            }
        }
    }
    return (Position){SIZE_MAX, SIZE_MAX};
}

// Finds the next path position by looking for the position with smaller
// time. If filling is true, we are instead looking for the next free
// position that has to be filled.
Position next_position(
    char **racetrack, size_t **times, Position current, bool filling
) {
    for (size_t i = 0; i < 4; ++i) {
        size_t x = current.x + neighbors[i][0];
        size_t y = current.y + neighbors[i][1];
        if ((!filling && times[y][x] < times[current.y][current.x])
            || (filling && racetrack[y][x] != '#' && times[y][x] == SIZE_MAX)) {
            return (Position){x, y};
        }
    }
    return current;
}

// Finds the path in the racetrack and computes time to reach each position.
void find_path(size_t rows, size_t cols, char **racetrack, size_t **times) {
    Position pos = find_position(rows, cols, racetrack, 'E');
    size_t time = 0;

    while (times[pos.y][pos.x] == SIZE_MAX) {
        times[pos.y][pos.x] = time;
        ++time;
        pos = next_position(racetrack, times, pos, true);
    }
}

// Returns the number of cheats starting from the given position that last
// at most duration picoseconds and save at least saving picoseconds.
size_t num_cheats(
    Position start, size_t rows, size_t cols, size_t **times, size_t duration,
    size_t saving
) {
    typedef struct State {
        Position pos;
        size_t steps;
    } State;

    bool **visited = init_visited(rows, cols, start);
    size_t time = times[start.y][start.x];

    State *q = 0;
    State state = {start, 0};
    arrput(q, state);

    size_t cheats = 0;
    while (arrlenu(q)) {
        State s = q[0];
        arrdel(q, 0);

        size_t t = times[s.pos.y][s.pos.x];
        // Found a non-wall position different from the starting one
        if (t != SIZE_MAX && s.steps) {
            // Check if path saves enough time
            cheats += t + s.steps < time && time - t - s.steps >= saving;
        }

        if (s.steps == duration) {
            continue;
        }

        // Continue the cheating path through walls
        for (size_t i = 0; i < 4; ++i) {
            Position pos = {
                .x = s.pos.x + neighbors[i][0], .y = s.pos.y + neighbors[i][1]
            };
            if (pos.y < rows && pos.x < cols && !visited[pos.y][pos.x]) {
                visited[pos.y][pos.x] = true;
                State next = {pos, s.steps + 1};
                arrput(q, next);
            }
        }
    }

    free_visited(rows, visited);
    return cheats;
}

// Returns the number of cheats that last at most duration picoseconds and
// save at least saving picoseconds.
size_t good_cheats(char **racetrack, size_t duration, size_t saving) {
    size_t rows = arrlen(racetrack);
    size_t cols = strlen(racetrack[0]);
    size_t **times = init_times(rows, cols);
    find_path(rows, cols, racetrack, times);

    size_t cheats = 0;
    Position pos = find_position(rows, cols, racetrack, 'S');
    while (times[pos.y][pos.x] != 0) {
        cheats += num_cheats(pos, rows, cols, times, duration, saving);
        pos = next_position(racetrack, times, pos, false);
    }

    free_times(rows, times);
    return cheats;
}
