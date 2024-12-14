#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 32
#define DAY 14

typedef struct Vector {
    int x;
    int y;
} Vector;

typedef struct Robot {
    Vector position;
    Vector velocity;
} Robot;

void test();
Robot *read_input(const char *filename);
size_t safety_factor(Robot *robots, int seconds, int width, int height);
size_t christmas_tree(Robot *robots, int width, int height);

int main(void) {
    test();

    Robot *robots = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", safety_factor(robots, 100, 101, 103));
    printf("%zu\n", christmas_tree(robots, 101, 103));
    arrfree(robots);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Robot *robots = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(safety_factor(robots, 100, 11, 7), 12);
    arrfree(robots);
}

// Parses the string and saves the coordinates written after '=' to x and y.
Vector parse_vector(char *str) {
    char *x_str = strchr(str, '=') + 1;
    char *y_str = strchr(x_str, ',') + 1;
    y_str[-1] = '\0';
    return (Vector){.x = atoi(x_str), .y = atoi(y_str)};
}

Robot *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Robot *robots = 0;
    while (fgets(buffer, MAX_LINE, file)) {
        char *velocity = strchr(buffer, ' ') + 1;
        velocity[-1] = '\0';
        Robot robot = {
            .position = parse_vector(buffer),
            .velocity = parse_vector(velocity),
        };
        arrput(robots, robot);
    }

    fclose(file);
    return robots;
}

// Returns the robots position after moving for the given amount of seconds
// in a grid with the given width and height.
Vector move_robot(Robot robot, int seconds, int width, int height) {
    Vector position = {
        .x = robot.position.x + robot.velocity.x * seconds,
        .y = robot.position.y + robot.velocity.y * seconds,
    };
    position.x = (position.x % width + width) % width;
    position.y = (position.y % height + height) % height;
    return position;
}

// Returns the safety factor, i.e., the product of the number of robots in
// each quadrant after the given amount of seconds.
size_t safety_factor(Robot *robots, int seconds, int width, int height) {
    size_t counts[2][2] = {0};

    for (size_t i = 0; i < arrlenu(robots); ++i) {
        Vector position = move_robot(robots[i], seconds, width, height);

        // Skip middle robots
        if (position.x == width / 2 || position.y == height / 2) {
            continue;
        }
        ++counts[position.y >= height / 2][position.x >= width / 2];
    }

    return counts[0][0] * counts[0][1] * counts[1][0] * counts[1][1];
}

double entropy(size_t n, int counts[n], size_t total) {
    double entropy = 0;
    for (size_t i = 0; i < n; ++i) {
        double p = (double) counts[i] / total;
        entropy -= p * (counts[i] ? log(p) : 0);
    }
    return entropy;
}

// Returns the time when the robots form a christmas tree.
size_t christmas_tree(Robot *robots, int width, int height) {
    double h_x = 0;
    double h_y = 0;

    for (int seconds = 1; seconds < width * height; ++seconds) {
        int *counts_x = calloc(width, sizeof(int));
        int *counts_y = calloc(height, sizeof(int));
        for (size_t i = 0; i < arrlenu(robots); ++i) {
            Vector position = move_robot(robots[i], seconds, width, height);
            ++counts_x[position.x];
            ++counts_y[position.y];
        }

        // Compute entropy for each dimension
        double entropy_x = entropy(width, counts_x, arrlenu(robots));
        double entropy_y = entropy(height, counts_y, arrlenu(robots));
        free(counts_x);
        free(counts_y);

        // If entropies lowered, the robots are not scattered randomly
        if (h_x && h_y && entropy_x < 0.95 * h_x && entropy_y < 0.95 * h_y) {
            return seconds;
        }

        h_x = entropy_x;
        h_y = entropy_y;
    }

    return -1;
}
