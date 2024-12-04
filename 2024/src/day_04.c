#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 256
#define DAY 04

typedef struct Grid {
    size_t rows;
    size_t cols;
    char **data;
} Grid;

void test();
Grid read_input(const char *filename);
void free_grid(Grid grid);
size_t occurrences(const Grid, const char *word);
size_t x_occurrences(const Grid);

int main(void) {
    test();

    Grid grid = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", occurrences(grid, "XMAS"));
    printf("%zu\n", x_occurrences(grid));
    free_grid(grid);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Grid grid = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(occurrences(grid, "XMAS"), 18);
    assert_eq(x_occurrences(grid), 9);
    free_grid(grid);
}

Grid read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Grid grid = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        char *new_line = strchr(buffer, '\n');
        if (new_line) {
            *new_line = '\0';
        }

        char *line = malloc(strlen(buffer) + 1);
        strcpy(line, buffer);
        arrput(grid.data, line);

        ++grid.rows;
        if (!grid.cols) {
            grid.cols = strlen(line);
        }
    }

    fclose(file);
    return grid;
}

void free_grid(Grid grid) {
    for (size_t i = 0; i < grid.rows; ++i) {
        free((void *) grid.data[i]);
    }
    arrfree(grid.data);
}

// Return true if there is a word match in the given grid, starting at (x, y)
// and stopping at (x + (len - 1) * dx, y + (len - 1) * dy).
bool word_match(
    const Grid grid, const char *word, int y, int x, int dy, int dx
) {
    size_t len = strlen(word);

    for (size_t i = 0; i < len; ++i) {
        int y_ = y + i * dy;
        int x_ = x + i * dx;
        bool valid_y = y_ >= 0 && y_ < (int) grid.rows;
        bool valid_x = x_ >= 0 && x_ < (int) grid.cols;

        if (!valid_y || !valid_x || grid.data[y_][x_] != word[i]) {
            return false;
        }
    }

    return true;
}

// Count the number of occurrences of the given word in the grid.
size_t occurrences(const Grid grid, const char *word) {
    size_t count = 0;
    size_t len = strlen(word);

    for (size_t i = 0; i < grid.rows; ++i) {
        for (size_t j = 0; j < grid.cols; ++j) {
            // Horizontal
            count += word_match(grid, word, i, j, 0, 1);
            count += word_match(grid, word, i, j + len - 1, 0, -1);

            // Vertical
            count += word_match(grid, word, i, j, 1, 0);
            count += word_match(grid, word, i + len - 1, j, -1, 0);

            // Positive diagonal
            count += word_match(grid, word, i, j, 1, 1);
            count += word_match(grid, word, i + len - 1, j + len - 1, -1, -1);

            // Negative diagonal
            count += word_match(grid, word, i + len - 1, j, -1, 1);
            count += word_match(grid, word, i, j + len - 1, 1, -1);
        }
    }

    return count;
}

// Count the number of occurrences of "MAS" cross in the grid.
size_t x_occurrences(const Grid grid) {
    size_t count = 0;
    char **g = grid.data;

    for (size_t i = 1; i < grid.rows - 1; ++i) {
        for (size_t j = 1; j < grid.cols - 1; ++j) {
            if (g[i][j] != 'A') {
                continue;
            }

            char tl = g[i - 1][j - 1];
            char tr = g[i - 1][j + 1];
            char bl = g[i + 1][j - 1];
            char br = g[i + 1][j + 1];

            bool a = tl == tr && tl == 'M' && bl == br && bl == 'S';
            bool b = tl == tr && tl == 'S' && bl == br && bl == 'M';
            bool c = tl == bl && tl == 'M' && tr == br && tr == 'S';
            bool d = tl == bl && tl == 'S' && tr == br && tr == 'M';
            count += a || b || c || d;
        }
    }

    return count;
}
