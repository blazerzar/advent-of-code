#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 64
#define DAY 08

// The last possible antenna is 'z' = 122
#define MAX_ANTENNAS 123

typedef struct Location {
    size_t x, y;
} Location;
typedef struct Map {
    size_t rows;
    size_t cols;
    Location *antennas[MAX_ANTENNAS];

} Map;

void test();
Map read_input(const char *filename);
void free_map(Map map);
size_t signal_impact(Map map, unsigned min_distance, unsigned max_distance);

int main(void) {
    test();

    Map map = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", signal_impact(map, 1, 1));
    printf("%zu\n", signal_impact(map, 0, -1));
    free_map(map);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Map map = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(signal_impact(map, 1, 1), 14);
    assert_eq(signal_impact(map, 0, -1), 34);
    free_map(map);
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

        ++map.rows;
        map.cols = strlen(buffer);
        for (size_t i = 0; i < map.cols; ++i) {
            if (buffer[i] != '.') {
                Location antenna = {i, map.rows - 1};
                arrput(map.antennas[(size_t) buffer[i]], antenna);
            }
        }
    }

    fclose(file);
    return map;
}

void free_map(Map map) {
    for (size_t i = 0; i < MAX_ANTENNAS; ++i) {
        if (map.antennas[i]) {
            arrfree(map.antennas[i]);
        }
    }
}

void mark_antinodes(
    Map map, Location a, Location delta, unsigned range[2], bool *antinodes
) {
    for (unsigned d = range[0]; d <= range[1]; ++d) {
        Location antinode = {a.x + d * delta.x, a.y + d * delta.y};
        if (antinode.x >= map.cols || antinode.y >= map.rows) {
            break;
        }
        antinodes[antinode.y * map.cols + antinode.x] = true;
    }
}

// Returns the number of unique locations within the bounds of the map that
// contain an antinode.
size_t signal_impact(Map map, unsigned min_distance, unsigned max_distance) {
    bool *antinodes = calloc(map.rows * map.cols, sizeof(bool));
    unsigned range[2] = {min_distance, max_distance};

    for (size_t antenna = 0; antenna < MAX_ANTENNAS; ++antenna) {
        if (!map.antennas[antenna]) {
            continue;
        }

        size_t size = arrlen(map.antennas[antenna]);
        for (size_t i = 0; i < size; ++i) {
            Location a = map.antennas[antenna][i];

            for (size_t j = i + 1; j < size; ++j) {
                Location b = map.antennas[antenna][j];
                ptrdiff_t dx = a.x - b.x;
                ptrdiff_t dy = a.y - b.y;

                mark_antinodes(map, a, (Location){dx, dy}, range, antinodes);
                mark_antinodes(map, b, (Location){-dx, -dy}, range, antinodes);
            }
        }
    }

    size_t impact = 0;
    for (size_t i = 0; i < map.rows * map.cols; ++i) {
        impact += antinodes[i];
    }

    free(antinodes);
    return impact;
}
