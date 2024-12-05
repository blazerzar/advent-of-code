#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 128
#define DAY 05

// Because the largest page is 99, we use a 100x100 grid
#define GRID_SIZE 100

typedef struct Pages {
    bool ordering[GRID_SIZE][GRID_SIZE];
    size_t **updates;
} Pages;

void test();
Pages read_input(const char *filename);
void free_pages(Pages pages);
size_t correctly_ordered_middle(Pages *pages);
size_t incorrectly_ordered_middle(Pages *pages);

int main(void) {
    test();

    Pages pages = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", correctly_ordered_middle(&pages));
    printf("%zu\n", incorrectly_ordered_middle(&pages));
    free_pages(pages);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Pages pages = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(correctly_ordered_middle(&pages), 143);
    assert_eq(incorrectly_ordered_middle(&pages), 123);
    free_pages(pages);
}

Pages read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Pages pages = {0};

    // Read ordering rules
    while (fgets(buffer, MAX_LINE, file) && buffer[0] != '\n') {
        buffer[2] = '\0';
        size_t left = atoi(buffer);
        size_t right = atoi(buffer + 3);
        pages.ordering[left][right] = true;
    }

    // Read updates
    while (fgets(buffer, MAX_LINE, file)) {
        size_t *update = 0;
        for (char *token = strtok(buffer, ","); token; token = strtok(0, ",")) {
            arrput(update, atoi(token));
        }
        arrput(pages.updates, update);
    }

    fclose(file);
    return pages;
}

void free_pages(Pages pages) {
    size_t len = arrlen(pages.updates);
    for (size_t i = 0; i < len; ++i) {
        arrfree(pages.updates[i]);
    }
    arrfree(pages.updates);
}

// Returns true if a should come before b in the ordering.
bool before(size_t a, size_t b, bool ordering[GRID_SIZE][GRID_SIZE]) {
    if (ordering[a][b]) {
        return true;
    } else if (ordering[b][a]) {
        return false;
    }

    for (size_t i = 0; i < GRID_SIZE; ++i) {
        if (ordering[a][i] && before(i, b, ordering)) {
            return true;
        }
    }

    return false;
}

// Returns true if the update array is correctly ordered according to the
// ordering rules.
bool correctly_ordered(size_t *update, bool ordering[GRID_SIZE][GRID_SIZE]) {
    size_t len = arrlen(update);
    for (size_t i = 0; i < len - 1; ++i) {
        for (size_t j = i + 1; j < len; ++j) {
            if (!before(update[i], update[j], ordering)) {
                return false;
            }
        }
    }

    return true;
}

// Returns the sum of the middle pages of correctly ordered updates.
size_t correctly_ordered_middle(Pages *pages) {
    size_t sum = 0;
    size_t len = arrlen(pages->updates);

    for (size_t i = 0; i < len; ++i) {
        if (correctly_ordered(pages->updates[i], pages->ordering)) {
            size_t middle = arrlen(pages->updates[i]) / 2;
            sum += pages->updates[i][middle];
        }
    }

    return sum;
}

typedef struct IntThunk {
    size_t value;
    bool (*ordering)[GRID_SIZE];
} IntThunk;

IntThunk *create_thunks(size_t *update, bool ordering[GRID_SIZE][GRID_SIZE]) {
    IntThunk *thunks = 0;
    size_t len = arrlen(update);
    for (size_t i = 0; i < len; ++i) {
        IntThunk thunk = {update[i], ordering};
        arrput(thunks, thunk);
    }
    return thunks;
}

int cmp_int_thunk(const void *a, const void *b) {
    IntThunk *ia = (IntThunk *) a;
    IntThunk *ib = (IntThunk *) b;
    bool is_before = before(ia->value, ib->value, ia->ordering);
    return is_before ? -1 : 1;
}

// Order the incorrectly ordered updates to conform to the ordering rules and
// return the sum of the middle pages after ordering.
size_t incorrectly_ordered_middle(Pages *pages) {
    size_t sum = 0;
    size_t len = arrlen(pages->updates);

    for (size_t i = 0; i < len; ++i) {
        if (!correctly_ordered(pages->updates[i], pages->ordering)) {
            IntThunk *thunks = create_thunks(
                pages->updates[i], pages->ordering
            );
            qsort(thunks, arrlen(thunks), sizeof(IntThunk), cmp_int_thunk);
            size_t middle = arrlen(pages->updates[i]) / 2;
            sum += thunks[middle].value;
            arrfree(thunks);
        }
    }

    return sum;
}
