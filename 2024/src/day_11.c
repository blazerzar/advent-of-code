#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 64
#define DAY 11

typedef struct Stone {
    size_t key;
    size_t value;
} Stone;

void test();
Stone *read_input(const char *filename);
void free_stones(Stone *stones);
size_t stones_after_blinking(Stone *stones, size_t blinks);

int main(void) {
    test();

    Stone *stones = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", stones_after_blinking(stones, 25));
    printf("%zu\n", stones_after_blinking(stones, 75));
    hmfree(stones);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Stone *stones = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(stones_after_blinking(stones, 25), 55312);
    hmfree(stones);
}

Stone *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Stone *stones = 0;
    size_t number;
    while (fscanf(file, "%zu", &number) == 1) {
        hmput(stones, number, 1);
    }

    fclose(file);
    return stones;
}

// Add multiple stones with the same number to the hashmap. If the stone
// already exists, increment the value, otherwise add a new stone.
void add_stone(Stone **stones, size_t number, size_t count) {
    ptrdiff_t index = hmgeti(*stones, number);
    if (index != -1) {
        (*stones)[index].value += count;
    } else {
        hmput(*stones, number, count);
    }
}

// Returns a new hashmap of stones after blinking once.
Stone *blink(Stone *stones) {
    Stone *new = 0;

    for (size_t i = 0; i < hmlenu(stones); ++i) {
        Stone s = stones[i];
        size_t digits = log10(s.key) + 1;

        if (s.key == 0) {
            add_stone(&new, 1, s.value);
        } else if (digits % 2 == 0) {
            size_t half_digits = pow(10, digits / 2);
            add_stone(&new, s.key / half_digits, s.value);
            add_stone(&new, s.key % half_digits, s.value);
        } else {
            add_stone(&new, s.key * 2024, s.value);
        }
    }

    return new;
}

// Returns the number of stones after blinking blinks times.
size_t stones_after_blinking(Stone *stones, size_t blinks) {
    for (size_t i = 0; i < blinks; ++i) {
        // Free only intermediate stones
        Stone *old = i ? stones : 0;
        stones = blink(stones);
        hmfree(old);
    }

    size_t count = 0;
    for (size_t i = 0; i < hmlenu(stones); ++i) {
        count += stones[i].value;
    }

    hmfree(stones);
    return count;
}
