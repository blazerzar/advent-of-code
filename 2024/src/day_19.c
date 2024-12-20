#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 4096
#define DAY 19

typedef struct Towels {
    struct {
        char *key;
        int value;
    } *towels;
    char **designs;
    size_t longest_towel;
} Towels;

void test();
Towels read_input(const char *filename);
void free_towels(Towels towels);
size_t possible_designs(Towels towels, bool count_ways);

int main(void) {
    test();

    Towels towels = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", possible_designs(towels, false));
    printf("%zu\n", possible_designs(towels, true));
    free_towels(towels);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Towels towels = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(possible_designs(towels, false), 6);
    assert_eq(possible_designs(towels, true), 16);
    free_towels(towels);
}

Towels read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Towels towels = {0};
    sh_new_strdup(towels.towels);

    char buffer[MAX_LINE];

    // Read towels
    fgets(buffer, MAX_LINE, file);
    buffer[strlen(buffer) - 1] = '\0';
    for (char *token = strtok(buffer, ", "); token; token = strtok(0, ", ")) {
        shput(towels.towels, token, 0);
        size_t len = strlen(token);
        if (len > towels.longest_towel) {
            towels.longest_towel = len;
        }
    }

    // Read designs
    while (fgets(buffer, MAX_LINE, file)) {
        size_t len = strlen(buffer);
        if (len > 1) {
            buffer[len - 1] = '\0';
            char *design = malloc(len);
            strcpy(design, buffer);
            arrput(towels.designs, design);
        }
    }

    fclose(file);
    return towels;
}

void free_towels(Towels towels) {
    shfree(towels.towels);
    for (size_t i = 0; i < arrlenu(towels.designs); ++i) {
        free(towels.designs[i]);
    }
    arrfree(towels.designs);
}

// Returns the number of towel combinations that form the given design.
size_t pattern_combinations_(char *design, Towels towels) {
    size_t len = strlen(design);
    size_t *combinations = calloc(len + 1, sizeof(size_t));
    combinations[len] = 1;

    for (size_t i = len; i > 0; --i) {
        char c = design[i];
        design[i] = '\0';
        int length_limit = i - towels.longest_towel;

        // Check if substring [j, i) is a towel
        for (int j = i - 1; j >= 0 && j >= length_limit; --j) {
            int towel = shgeti(towels.towels, design + j);
            if (towel != -1) {
                combinations[j] += combinations[i];
            }
        }

        design[i] = c;
    }

    size_t result = combinations[0];
    free(combinations);
    return result;
}

// Returns the number of possible designs that can be formed with the given
// set of towels or the number of ways to form them.
size_t possible_designs(Towels towels, bool count_ways) {
    size_t possible = 0;

    for (size_t i = 0; i < arrlenu(towels.designs); ++i) {
        size_t res = pattern_combinations_(towels.designs[i], towels);
        possible += count_ways ? res : (res > 0);
    }

    return possible;
}
