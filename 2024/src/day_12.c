#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 256
#define DAY 12

typedef struct Garden {
    char **plants;
    size_t rows;
    size_t cols;
} Garden;

ptrdiff_t neighbors[4][2] = {
    {-1, 0 },
    {1,  0 },
    {0,  -1},
    {0,  1 }
};

void test();
Garden read_input(const char *filename);
void free_garden(Garden garden);
size_t fencing_price(Garden garden, bool bulk_discount);

int main(void) {
    test();

    Garden garden = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", fencing_price(garden, false));
    printf("%zu\n", fencing_price(garden, true));
    free_garden(garden);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Garden garden = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(fencing_price(garden, false), 1930);
    assert_eq(fencing_price(garden, true), 1206);
    free_garden(garden);
}

Garden read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Garden garden = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        char *line_end = strchr(buffer, '\n');
        if (line_end) {
            *line_end = '\0';
        }

        if (!garden.cols) {
            garden.cols = strlen(buffer);
        }

        char *line = malloc(garden.cols + 1);
        strcpy(line, buffer);
        arrput(garden.plants, line);
        ++garden.rows;
    }

    fclose(file);
    return garden;
}

void free_garden(Garden garden) {
    for (size_t i = 0; i < garden.rows; ++i) {
        free(garden.plants[i]);
    }
    arrfree(garden.plants);
}

// Returns the number of plot corners around the plant at position (x, y).
size_t plot_corners(Garden garden, size_t x, size_t y) {
    char plant = garden.plants[y][x];
    bool neighborhood[3][3] = {0};

    // Check 9-neighborhood around the plant for the same plant
    for (ptrdiff_t dx = -1; dx <= 1; ++dx) {
        for (ptrdiff_t dy = -1; dy <= 1; ++dy) {
            size_t x_ = x + dx;
            size_t y_ = y + dy;
            if (x_ >= garden.cols || y_ >= garden.rows) {
                continue;
            }
            neighborhood[dy + 1][dx + 1] = garden.plants[y_][x_] == plant;
        }
    }

    size_t corners = 0;

    // Go over each corner and check the neighboring two plants
    for (size_t i = 0; i < 3; i += 2) {
        bool a = neighborhood[i][1];

        for (size_t j = 0; j < 3; j += 2) {
            bool c = neighborhood[i][j];
            bool b = neighborhood[1][j];

            // If the corner (c) is a different plant, the neighbors (a, b)
            // have to be both the same or both different plants. If the
            // corner is the same plant, the neighbors have to be both
            // different plants.
            corners += (!c && a == b) || (c && !a && !b);
        }
    }

    return corners;
}

// Returns the area, perimeter and the number of sides (corners) of the garden
// plot containing the plant at position (x, y). We mark the visited plants to
// avoid counting the same plot multiple times.
void plot_dimensions(
    Garden garden, size_t x, size_t y, bool *visited, size_t *area,
    size_t *perimeter, size_t *sides
) {
    char plant = garden.plants[y][x];
    visited[y * garden.cols + x] = true;
    *area += 1;
    *sides += plot_corners(garden, x, y);

    for (size_t i = 0; i < 4; ++i) {
        size_t x_ = x + neighbors[i][0];
        size_t y_ = y + neighbors[i][1];

        if (x_ >= garden.cols || y_ >= garden.rows
            || garden.plants[y_][x_] != plant) {
            *perimeter += 1;
        } else if (!visited[y_ * garden.cols + x_]) {
            plot_dimensions(garden, x_, y_, visited, area, perimeter, sides);
        }
    }
}

// Returns the price of the fencing by summing the products of the area and
// perimeter of each garden plot. With bulk discount, the price is calculated
// using the number of sides instead of the perimeter.
size_t fencing_price(Garden garden, bool bulk_discount) {
    size_t price = 0;
    bool *visited = calloc(garden.rows * garden.cols, sizeof(bool));

    for (size_t y = 0; y < garden.rows; ++y) {
        for (size_t x = 0; x < garden.cols; ++x) {
            if (visited[y * garden.cols + x]) {
                continue;
            }

            size_t area = 0;
            size_t perimeter = 0;
            size_t sides = 0;
            plot_dimensions(garden, x, y, visited, &area, &perimeter, &sides);
            price += area * (bulk_discount ? sides : perimeter);
        }
    }

    free(visited);
    return price;
}
