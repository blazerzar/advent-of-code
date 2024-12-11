#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 64
#define DAY 10

typedef struct Map {
    char **map;
    size_t rows;
    size_t cols;
} Map;

ptrdiff_t neighbors[4][2] = {
    {0,  -1},
    {1,  0 },
    {0,  1 },
    {-1, 0 },
};

void test();
Map read_input(const char *filename);
void free_map(Map map);
size_t scores_ratings(Map map, size_t *ratings);

int main(void) {
    test();

    Map map = read_input("inputs/day_" STR(DAY) ".txt");
    size_t ratings = 0;
    printf("%zu\n", scores_ratings(map, &ratings));
    printf("%zu\n", ratings);
    free_map(map);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Map map = read_input("inputs/day_" STR(DAY) "_test.txt");
    size_t ratings = 0;
    assert_eq(scores_ratings(map, &ratings), 36);
    assert_eq(ratings, 81);
    free_map(map);
}

Map read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Map map = {0};
    char buffer[MAX_LINE];
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
    }

    fclose(file);
    return map;
}

void free_map(Map map) {
    for (size_t i = 0; i < map.rows; ++i) {
        free((void *) map.map[i]);
    }
    arrfree(map.map);
}

// Visits all possible positions and marks visited 9-height positions. It
// returns the number of distinct hiking trails that lead to these positions.
size_t visit(Map map, size_t x, size_t y, bool *visited) {
    char height = map.map[y][x];
    if (height == '9') {
        visited[y * map.cols + x] = true;
        return 1;
    }

    size_t trails = 0;
    for (size_t i = 0; i < 4; ++i) {
        size_t x_ = x + neighbors[i][0];
        size_t y_ = y + neighbors[i][1];
        if (y_ >= map.rows || x_ >= map.cols || map.map[y_][x_] - height != 1) {
            continue;
        }
        trails += visit(map, x_, y_, visited);
    }
    return trails;
}

// Returns the score of the trailhead at position (x, y), i.e., the number of
// 9-height positions reachable from it. The trailhead rating is saved into
// the rating pointer, i.e., the number of distinct hiking trails that lead to
// these positions.
size_t trailhead_score_rating(Map map, size_t x, size_t y, size_t *rating) {
    if (map.map[y][x] != '0') {
        return 0;
    }

    bool *visited = calloc(map.rows * map.cols, sizeof(bool));
    *rating = visit(map, x, y, visited);

    size_t score = 0;
    for (size_t i = 0; i < map.rows * map.cols; ++i) {
        score += visited[i];
    }

    free(visited);
    return score;
}

// Returns the sum of the scores and ratings of all trailheads.
size_t scores_ratings(Map map, size_t *ratings) {
    size_t scores = 0;
    for (size_t i = 0; i < map.rows; ++i) {
        for (size_t j = 0; j < map.cols; ++j) {
            size_t rating = 0;
            scores += trailhead_score_rating(map, j, i, &rating);
            *ratings += rating;
        }
    }
    return scores;
}
