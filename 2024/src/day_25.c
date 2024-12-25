#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 8
#define DAY 25

#define COLUMNS 5

typedef struct Schematics {
    size_t *locks;
    size_t *keys;
} Schematics;

void test();
Schematics read_input(const char *filename);
void free_schematics(Schematics *schematics);
size_t lock_key_pairs(Schematics schematics);

int main(void) {
    test();

    Schematics schematics = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", lock_key_pairs(schematics));
    free_schematics(&schematics);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Schematics schematics = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(lock_key_pairs(schematics), 3);
    free_schematics(&schematics);
}

size_t parse_schematic(FILE *file) {
    char buffer[MAX_LINE];

    // Each column of lock/key is repesented by a decimal digit.
    size_t schematic = 100000;
    while (fgets(buffer, MAX_LINE, file) && strlen(buffer) > 1) {
        for (size_t i = 0; i < COLUMNS; ++i) {
            schematic += (buffer[i] == '#') * (size_t) pow(10, COLUMNS - 1 - i);
        }
    }

    return schematic;
}

Schematics read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Schematics schematics = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        bool is_lock = buffer[0] == '#';
        size_t schematic = parse_schematic(file);
        if (is_lock) {
            arrput(schematics.locks, schematic);
        } else {
            // Because the last row is filled and parsed, we subtract 1 from
            // each column
            arrput(schematics.keys, schematic - 11111);
        }
    }

    fclose(file);
    return schematics;
}

void free_schematics(Schematics *schematics) {
    arrfree(schematics->locks);
    arrfree(schematics->keys);
}

// Returns the number of unique lock/key pairs that fit together.
size_t lock_key_pairs(Schematics schematics) {
    size_t count = 0;

    for (size_t i = 0; i < arrlenu(schematics.locks); ++i) {
        for (size_t j = 0; j < arrlenu(schematics.keys); ++j) {
            size_t lock = schematics.locks[i];
            size_t key = schematics.keys[j];
            bool fits = true;
            for (size_t k = 0; k < COLUMNS; ++k) {
                fits &= lock % 10 + key % 10 <= 5;
                lock /= 10;
                key /= 10;
            }
            count += fits;
        }
    }

    return count;
}
