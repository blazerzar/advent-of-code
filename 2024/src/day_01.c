#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 1024
#define DAY 01

typedef struct Locations {
    size_t length;
    int *left;
    int *right;
} Locations;

void test();
Locations read_input(const char *filename);
void free_locations(Locations locations);
int lists_difference(Locations locations);
int lists_similarity(Locations locations);

int main(void) {
    test();

    Locations locations = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%d\n", lists_difference(locations));
    printf("%d\n", lists_similarity(locations));
    free_locations(locations);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Locations locations = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(lists_difference(locations), 11);
    assert_eq(lists_similarity(locations), 31);
    free_locations(locations);
}

Locations read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Locations locations = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        int left, right;
        if (sscanf(buffer, "%d %d", &left, &right) != 2) {
            fprintf(stderr, "Invalid input: %s", buffer);
            exit(EXIT_FAILURE);
        }

        arrput(locations.left, left);
        arrput(locations.right, right);
        ++locations.length;
    }

    fclose(file);
    return locations;
}

void free_locations(Locations locations) {
    arrfree(locations.left);
    arrfree(locations.right);
}

int comparator(const void *a, const void *b) {
    return (*(int *) a - *(int *) b);
}

// Compute the difference between two lists of location IDs by sorting them
// and then comparing them element by element.
int lists_difference(Locations locations) {
    qsort(locations.left, locations.length, sizeof(int), comparator);
    qsort(locations.right, locations.length, sizeof(int), comparator);

    int difference = 0;
    for (size_t i = 0; i < locations.length; ++i) {
        difference += abs(locations.left[i] - locations.right[i]);
    }

    return difference;
}

// Compute the similarity between two lists of location IDs by summing up the
// numbers from the left list after multiplying them with the number of
// occurrences in the right list. We assume the lists are sorted from the
// function lists_difference.
int lists_similarity(Locations locations) {
    int similarity = 0;

    size_t last_right = 0;
    int count = 0;

    for (size_t i = 0; i < locations.length; ++i) {
        // The while loop will only count if the next number is different
        if (i > 0 && locations.left[i] != locations.left[i - 1]) {
            count = 0;
        }

        while (last_right < locations.length
               && locations.right[last_right] <= locations.left[i]) {
            count += locations.right[last_right] == locations.left[i];
            ++last_right;
        }

        similarity += locations.left[i] * count;
    }

    return similarity;
}
