#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 1024
#define DAY 15

typedef enum Direction { UP, DOWN, RIGHT, LEFT, DIRS } Direction;

typedef struct Location {
    size_t x, y;
} Location;

typedef struct Warehouse {
    char **map;
    size_t rows;
    size_t cols;
    Direction *instructions;
    Location robot;
} Warehouse;

ptrdiff_t neighbors[4][2] = {
    [UP] = {0,  -1},
    [DOWN] = {0,  1 },
    [RIGHT] = {1,  0 },
    [LEFT] = {-1, 0 },
};

void test();
Warehouse read_input(const char *filename);
void free_warehouse(Warehouse warehouse);
size_t boxes_gps(Warehouse warehouse);
size_t boxes_gps_large(Warehouse warehouse);

int main(void) {
    test();

    Warehouse warehouse = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", boxes_gps(warehouse));
    printf("%zu\n", boxes_gps_large(warehouse));
    free_warehouse(warehouse);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Warehouse warehouse = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    assert_eq(boxes_gps(warehouse), 2028);
    free_warehouse(warehouse);

    warehouse = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    assert_eq(boxes_gps(warehouse), 10092);
    assert_eq(boxes_gps_large(warehouse), 9021);
    free_warehouse(warehouse);
}

void parse_map(FILE *file, Warehouse *warehouse) {
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file) && strlen(buffer) > 1) {
        if (!warehouse->cols) {
            warehouse->cols = strlen(buffer) - 1;
        }

        buffer[warehouse->cols] = '\0';
        char *line = malloc(warehouse->cols + 1);
        strcpy(line, buffer);
        arrput(warehouse->map, line);
        ++warehouse->rows;
    }
}

void parse_instructions(FILE *file, Warehouse *warehouse) {
    const char *dirs = "^v><";
    char buffer[MAX_LINE];

    while (fgets(buffer, MAX_LINE, file)) {
        size_t len = strlen(buffer) - 1;
        for (size_t i = 0; i < len; ++i) {
            arrput(warehouse->instructions, strchr(dirs, buffer[i]) - dirs);
        }
    }
}

Warehouse read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Warehouse warehouse = {0};
    parse_map(file, &warehouse);
    parse_instructions(file, &warehouse);

    // Find the starting position and clear it
    for (size_t i = 0; i < warehouse.rows; ++i) {
        for (size_t j = 0; j < warehouse.cols; ++j) {
            if (warehouse.map[i][j] == '@') {
                warehouse.map[i][j] = '.';
                warehouse.robot = (Location){.x = j, .y = i};
            }
        }
    }

    fclose(file);
    return warehouse;
}

void free_warehouse(Warehouse warehouse) {
    for (size_t i = 0; i < warehouse.rows; ++i) {
        free(warehouse.map[i]);
    }
    arrfree(warehouse.map);
    arrfree(warehouse.instructions);
}

Warehouse copy_warehouse(Warehouse warehouse) {
    Warehouse copy = {0};
    copy.rows = warehouse.rows;
    copy.cols = warehouse.cols;
    copy.robot = warehouse.robot;

    for (size_t i = 0; i < warehouse.rows; ++i) {
        char *line = malloc(warehouse.cols + 1);
        strcpy(line, warehouse.map[i]);
        arrput(copy.map, line);
    }

    for (size_t i = 0; i < arrlenu(warehouse.instructions); ++i) {
        arrput(copy.instructions, warehouse.instructions[i]);
    }

    return copy;
}

// Makes the warehouse twice as wide.
Warehouse enlarge_warehouse(Warehouse warehouse) {
    Warehouse enlarged = {0};
    enlarged.rows = warehouse.rows;
    enlarged.cols = warehouse.cols * 2;
    enlarged.robot.x = warehouse.robot.x * 2;
    enlarged.robot.y = warehouse.robot.y;

    for (size_t i = 0; i < enlarged.rows; ++i) {
        char *line = calloc(enlarged.cols + 1, sizeof(char));
        for (size_t j = 0; j < warehouse.cols; ++j) {
            char left = warehouse.map[i][j];
            char right = left;
            if (warehouse.map[i][j] == 'O') {
                left = '[';
                right = ']';
            }
            line[j * 2] = left;
            line[j * 2 + 1] = right;
        }
        arrput(enlarged.map, line);
    }

    for (size_t i = 0; i < arrlenu(warehouse.instructions); ++i) {
        arrput(enlarged.instructions, warehouse.instructions[i]);
    }

    return enlarged;
}

// Returns the sum of boxes' GPS coordinates, i.e., 100 * y + x.
size_t coordinates_sum(Warehouse warehouse, char box) {
    size_t sum = 0;
    for (size_t i = 0; i < warehouse.rows; ++i) {
        for (size_t j = 0; j < warehouse.cols; ++j) {
            if (warehouse.map[i][j] == box) {
                sum += 100 * i + j;
            }
        }
    }
    return sum;
}

Location next_location(Location location, Direction instruction) {
    return (Location){
        .x = location.x + neighbors[instruction][0],
        .y = location.y + neighbors[instruction][1],
    };
}

void move_robot(Warehouse *warehouse, Direction instruction) {
    Location next = next_location(warehouse->robot, instruction);
    Location empty = next;

    // Find the next empty space
    while (warehouse->map[empty.y][empty.x] == 'O') {
        empty = next_location(empty, instruction);
    }

    // If there is no empty space, the robot does not move
    if (warehouse->map[empty.y][empty.x] == '#') {
        return;
    }

    // If the empty space is not next to the robot, a box is moved
    if (next.x != empty.x || next.y != empty.y) {
        warehouse->map[empty.y][empty.x] = 'O';
        warehouse->map[next.y][next.x] = '.';
    }

    warehouse->robot = next;
}

// Moves the box with one part at the given location. The conditions for
// moving should already be checked.
void move_box(Warehouse warehouse, Location loc, Direction dir) {
    char c = warehouse.map[loc.y][loc.x];
    warehouse.map[loc.y][loc.x] = '.';

    // Move the box part at loc
    Location next = next_location(loc, dir);
    warehouse.map[next.y][next.x] = c;

    // Move the other box part
    if (dir == RIGHT || dir == LEFT) {
        next = next_location(next, dir);
    } else {
        next = next_location(next, c == '[' ? RIGHT : LEFT);

        // Clear the adjacent space
        loc = next_location(loc, c == '[' ? RIGHT : LEFT);
        warehouse.map[loc.y][loc.x] = '.';
    }
    warehouse.map[next.y][next.x] = c == '[' ? ']' : '[';
}

// Returns true if the given space is clear or can be made clear by moving
// some boxes. If move is true, the boxes are also moved.
bool clear_space(Warehouse warehouse, Location loc, Direction dir, bool move) {
    char c = warehouse.map[loc.y][loc.x];
    if (c != '[' && c != ']') {
        return c == '.';
    }

    bool clear = false;
    Location next = next_location(loc, dir);
    if (dir == RIGHT || dir == LEFT) {
        // The space after the box must be clear
        next = next_location(next, dir);
        clear = clear_space(warehouse, next, dir, move);
    } else {
        // Two spaces after the box must be clear
        clear = clear_space(warehouse, next, dir, move);
        next = next_location(next, c == '[' ? RIGHT : LEFT);
        clear &= clear_space(warehouse, next, dir, move);
    }

    if (clear && move) {
        move_box(warehouse, loc, dir);
    }
    return clear;
}

// Returns the sum of all boxes' GPS coordinates in the small warehouse.
size_t boxes_gps(Warehouse warehouse) {
    warehouse = copy_warehouse(warehouse);
    for (size_t i = 0; i < arrlenu(warehouse.instructions); ++i) {
        move_robot(&warehouse, warehouse.instructions[i]);
    }

    size_t coordinates = coordinates_sum(warehouse, 'O');
    free_warehouse(warehouse);
    return coordinates;
}

// Returns the sum of all boxes' GPS coordinates in the large warehouse.
size_t boxes_gps_large(Warehouse warehouse) {
    warehouse = enlarge_warehouse(warehouse);
    for (size_t i = 0; i < arrlenu(warehouse.instructions); ++i) {
        Direction instruction = warehouse.instructions[i];
        Location next = next_location(warehouse.robot, instruction);

        // First, check if the space can be cleared, ...
        if (clear_space(warehouse, next, instruction, false)) {
            // ... then clear it and move the robot
            clear_space(warehouse, next, instruction, true);
            warehouse.robot = next;
        }
    }

    size_t coordinates = coordinates_sum(warehouse, '[');
    free_warehouse(warehouse);
    return coordinates;
}
