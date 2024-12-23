#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 16
#define DAY 22

typedef struct KeyVal {
    size_t key;
    size_t value;
} KeyVal;

void test();
size_t *read_input(const char *filename);
size_t generate_secret_number(size_t secret, size_t iterations);
size_t buyers_secret_numbers(size_t *secrets);
size_t buy_bananas(size_t *secrets);
size_t seq_to_num(int sequence[4]);

int main(void) {
    test();

    size_t *secrets = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", buyers_secret_numbers(secrets));
    printf("%zu\n", buy_bananas(secrets));
    arrfree(secrets);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    size_t *secrets = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    assert_eq(generate_secret_number(123, 1), 15887950);
    assert_eq(generate_secret_number(123, 2), 16495136);
    assert_eq(generate_secret_number(123, 10), 5908254);
    assert_eq(buyers_secret_numbers(secrets), 37327623);
    arrfree(secrets);

    assert_eq(seq_to_num((int[]){-2, 4, -9, 0}), 108140110);
    assert_eq(seq_to_num((int[]){-5, 0, 0, 9}), 105101019);

    secrets = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    assert_eq(buy_bananas(secrets), 23);
    arrfree(secrets);
}

size_t *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    size_t *numbers = 0;
    while (fgets(buffer, MAX_LINE, file)) {
        arrput(numbers, strtoul(buffer, 0, 10));
    }

    fclose(file);
    return numbers;
}

size_t mix(size_t secret, size_t number) {
    return secret ^ number;
}

size_t prune(size_t secret) {
    return secret % 16777216;
}

// Returns the secret number after the given number of iterations.
size_t generate_secret_number(size_t secret, size_t iterations) {
    for (size_t i = 0; i < iterations; ++i) {
        secret = prune(mix(secret, secret * 64));
        secret = prune(mix(secret, secret / 32));
        secret = prune(mix(secret, secret * 2048));
    }
    return secret;
}

// Returns the sum of the 2000th secret numbers of the buyers.
size_t buyers_secret_numbers(size_t *secrets) {
    size_t sum = 0;
    for (size_t i = 0; i < arrlenu(secrets); ++i) {
        sum += generate_secret_number(secrets[i], 2000);
    }
    return sum;
}

// Encodes a sequence of differences in range [-9, 9] to a number by adding 10
// to each difference and concatenating them into a number like 1aabbccdd.
size_t seq_to_num(int sequence[4]) {
    size_t num = 0;
    for (size_t i = 0; i < 4; ++i) {
        num = num * 100 + sequence[i] + 10;
    }
    return num + 100000000;
}

// Returns the initial encoding of the first 4 prices. The first difference
// is set to 0.
size_t initial_sequence(int *prices) {
    int diffs[4] = {0};
    for (size_t i = 1; i < 4; ++i) {
        diffs[i] = prices[i] - prices[i - 1];
    }
    return seq_to_num(diffs);
}

// Generates the first prices that do not form a sequence yet and returns the
// last secret value.
size_t initial_prices(size_t secret, int **prices) {
    arrput(*prices, secret % 10);
    for (size_t iter = 1; iter < 4; ++iter) {
        secret = generate_secret_number(secret, 1);
        arrput(*prices, secret % 10);
    }
    return secret;
}

// Returns the most bananas we can get by instructing the monkey to sell the
// first time each buyer's sequence of price changes matches some sequence.
size_t buy_bananas(size_t *secrets) {
    KeyVal *seq_bananas = 0;
    size_t max_bananas = 0;

    for (size_t i = 0; i < arrlenu(secrets); ++i) {
        size_t secret = secrets[i];
        KeyVal *seen_sequences = 0;

        int *prices = 0;
        arrsetcap(prices, 2001);
        secret = initial_prices(secret, &prices);
        size_t seq = initial_sequence(prices);

        for (size_t j = 4; j <= 2000; ++j) {
            secret = generate_secret_number(secret, 1);
            int price = secret % 10;
            int diff = price - prices[j - 1];
            arrput(prices, price);

            // Remove the first difference from sequence and add the new one
            seq = 100000000 + (seq % 1000000) * 100 + diff + 10;

            // Has to be the first occurrence of the sequence
            if (hmgeti(seen_sequences, seq) == -1) {
                hmput(seen_sequences, seq, 1);

                // Update the number of bananas gotten for this sequence
                size_t bananas = hmget(seq_bananas, seq) + price;
                hmput(seq_bananas, seq, bananas);
                max_bananas = max_bananas > bananas ? max_bananas : bananas;
            }
        }

        arrfree(prices);
        hmfree(seen_sequences);
    }

    hmfree(seq_bananas);
    return max_bananas;
}
