#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 64
#define DAY 13

typedef struct Location {
    long long x, y;
} Location;

typedef struct Machine {
    Location button_a, button_b, prize;
} Machine;

void test();
Machine *read_input(const char *filename);
size_t win_prizes(Machine *machines, size_t offset);

int main(void) {
    test();

    Machine *machines = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", win_prizes(machines, 0));
    printf("%zu\n", win_prizes(machines, 10000000000000));
    arrfree(machines);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Machine *machines = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(win_prizes(machines, 0), 480);
    arrfree(machines);
}

Machine *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Machine *machines = 0;
    while (fgets(buffer, MAX_LINE, file)) {
        Machine machine = {0};

        for (size_t i = 0; i < 3; ++i) {
            char *x = strchr(buffer, 'X') + 2;
            char *y = strchr(x, 'Y') + 2;
            *(y - 4) = '\0';

            Location location = {.x = atoi(x), .y = atoi(y)};
            if (i == 0) {
                machine.button_a = location;
            } else if (i == 1) {
                machine.button_b = location;
            } else {
                machine.prize = location;
            }

            fgets(buffer, MAX_LINE, file);
        }

        arrput(machines, machine);
    }

    fclose(file);
    return machines;
}

// Returns the fewest number of tokens needed to win the prize. If the prize
// is not obtainable, returns 0. We solve a linear system to find the linear
// combination of vectors A and B that equals the prize vector. If the
// solution is not integral, the prize is not obtainable.
size_t machine_tokens(Machine m) {
    // 2D matrix system solution
    long long det = m.button_a.x * m.button_b.y - m.button_a.y * m.button_b.x;
    long long a = m.button_b.y * m.prize.x - m.button_b.x * m.prize.y;
    long long b = m.button_a.x * m.prize.y - m.button_a.y * m.prize.x;

    if (a % det || b % det) {
        return 0;
    }

    return (3 * a + b) / det;
}

// Returns the fewest number of tokens needed to win all possible prizes.
size_t win_prizes(Machine *machines, size_t offset) {
    size_t tokens = 0;

    for (size_t i = 0; i < arrlenu(machines); ++i) {
        Machine m = machines[i];
        m.prize.x += offset;
        m.prize.y += offset;
        tokens += machine_tokens(m);
    }

    return tokens;
}
