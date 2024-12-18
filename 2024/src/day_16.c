#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 256
#define DAY 16
#define HEAP_SIZE 10000

typedef struct Maze {
    char **map;
    size_t rows;
    size_t cols;
} Maze;

typedef enum Direction { UP, DOWN, LEFT, RIGHT, NUM_DIRS } Direction;
typedef struct Location {
    size_t x, y;
} Location;
typedef struct State {
    Location loc;
    Direction dir;
    size_t score;
    Location *path;
} State;

typedef struct MinHeap {
    State data[HEAP_SIZE];
    size_t size;
} MinHeap;

ptrdiff_t neighbors[4][2] = {
    [UP] = {0,  -1},
    [DOWN] = {0,  1 },
    [LEFT] = {-1, 0 },
    [RIGHT] = {1,  0 },
};

void test();
Maze read_input(const char *filename);
void free_maze(Maze maze);
size_t best_score(Maze maze, size_t *best_tiles);

void enqueue(MinHeap *, Location, Direction, size_t, Location *);
State dequeue(MinHeap *);

int main(void) {
    test();

    Maze maze = read_input("inputs/day_" STR(DAY) ".txt");
    size_t best_tiles;
    printf("%zu\n", best_score(maze, &best_tiles));
    printf("%zu\n", best_tiles);
    free_maze(maze);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Maze maze = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    size_t best_tiles;
    assert_eq(best_score(maze, &best_tiles), 7036);
    assert_eq(best_tiles, 45);
    free_maze(maze);

    maze = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    assert_eq(best_score(maze, &best_tiles), 11048);
    assert_eq(best_tiles, 64);
    free_maze(maze);
}

Maze read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Maze maze = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        if (!maze.cols) {
            maze.cols = strlen(buffer) - 1;
        }
        buffer[maze.cols] = '\0';
        char *line = malloc(maze.cols + 1);
        strcpy(line, buffer);
        arrput(maze.map, line);
        ++maze.rows;
    }

    fclose(file);
    return maze;
}

void free_maze(Maze maze) {
    for (size_t i = 0; i < maze.rows; ++i) {
        free(maze.map[i]);
    }
    arrfree(maze.map);
}

// Returns a duplicate of the given array.
Location *arr_dup(Location *arr) {
    Location *dup = 0;
    for (size_t i = 0; i < arrlenu(arr); ++i) {
        arrput(dup, arr[i]);
    }
    return dup;
}

void mark_tiles(Maze maze, Location *path, bool tiles[maze.rows * maze.cols]) {
    // Mark start and end
    tiles[(maze.rows - 2) * maze.cols + 1] = true;
    tiles[1 * maze.cols + maze.cols - 2] = true;

    for (size_t i = 0; i < arrlenu(path); ++i) {
        Location loc = path[i];
        tiles[loc.y * maze.cols + loc.x] = true;
    }
}

size_t score_index(Maze maze, Location loc, Direction dir) {
    return (loc.y * maze.cols + loc.x) * NUM_DIRS + dir;
}

bool better_score(size_t score, size_t prev_score, size_t best_score) {
    return score < best_score && (!prev_score || prev_score > score);
}

// Finds all the best paths, marks their tiles and returns the lowest score.
size_t best_path(Maze maze, bool tiles[maze.rows * maze.cols]) {
    size_t *scores = calloc(maze.rows * maze.cols * NUM_DIRS, sizeof(size_t));
    size_t best = -1;

    MinHeap *q = calloc(1, sizeof(MinHeap));
    enqueue(q, (Location){1, maze.rows - 2}, RIGHT, 0, 0);

    while (q->size) {
        State s = dequeue(q);
        scores[score_index(maze, s.loc, s.dir)] = s.score;

        // If we come to the end, one of the best paths is found
        if (maze.map[s.loc.y][s.loc.x] == 'E') {
            mark_tiles(maze, s.path, tiles);
            best = s.score;
            arrfree(s.path);
            continue;
        }

        // Try rotating
        for (size_t i = 1; i < NUM_DIRS; ++i) {
            size_t score = s.score + 1000;
            Direction dir = (s.dir + i) % NUM_DIRS;
            size_t prev_score = scores[score_index(maze, s.loc, dir)];
            if (better_score(score, prev_score, best)) {
                enqueue(q, s.loc, dir, score, arr_dup(s.path));
            }
        }

        // Try moving forward
        size_t x = s.loc.x + neighbors[s.dir][0];
        size_t y = s.loc.y + neighbors[s.dir][1];
        Location loc = {x, y};
        size_t score = s.score + 1;
        size_t prev_score = scores[score_index(maze, loc, s.dir)];
        if (maze.map[y][x] != '#' && better_score(score, prev_score, best)) {
            Location *path = arr_dup(s.path);
            arrput(path, loc);
            enqueue(q, loc, s.dir, score, path);
        }

        arrfree(s.path);
    }

    free(scores);
    free(q);
    return best;
}

// Returns the lowest score the Reindeer could possibly get and saves the
// number of tiles that are part of at least one of the best paths.
size_t best_score(Maze maze, size_t *best_tiles) {
    bool *tiles = calloc(maze.rows * maze.cols, sizeof(bool));
    size_t score = best_path(maze, tiles);

    *best_tiles = 0;
    for (size_t i = 0; i < maze.rows * maze.cols; ++i) {
        *best_tiles += tiles[i];
    }

    free(tiles);
    return score;
}

State dequeue(MinHeap *heap) {
    State min = heap->data[0];
    heap->data[0] = heap->data[--heap->size];

    for (size_t i = 0; 2 * i + 1 < heap->size;) {
        size_t min = 1;
        size_t left_score = heap->data[2 * i + min].score;
        size_t right = 2 * i + 2;
        min += right < heap->size && heap->data[right].score < left_score;

        if (heap->data[i].score <= heap->data[2 * i + min].score) {
            break;
        }

        State tmp = heap->data[i];
        heap->data[i] = heap->data[2 * i + min];
        heap->data[2 * i + min] = tmp;
        i = 2 * i + min;
    }

    return min;
}

void enqueue(
    MinHeap *heap, Location loc, Direction dir, size_t score, Location *path
) {
    size_t i = heap->size++;
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (score >= heap->data[parent].score) {
            break;
        }

        heap->data[i] = heap->data[parent];
        i = parent;
    }
    heap->data[i] = (State){loc, dir, score, path};
}
