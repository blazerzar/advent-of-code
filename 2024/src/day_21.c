#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 8
#define DAY 21
#define CODES 5
#define CODE_LENGTH 4

typedef struct Memory {
    size_t key;
    size_t value;
} Memory;

typedef struct Position {
    size_t x;
    size_t y;
} Position;

char keypads[6][4] = {
    {'7', '8', '9'},
    {'4', '5', '6'},
    {'1', '2', '3'},
    {' ', '0', 'A'},
    {' ', '^', 'A'},
    {'<', 'v', '>'},
};

void test();
char **read_input(const char *filename);
void free_codes(char **codes);
Position keypad_position(char button, bool numeric);
size_t complexities_sum(char **codes, size_t keypads);

int main(void) {
    test();

    char **codes = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", complexities_sum(codes, 2));
    printf("%zu\n", complexities_sum(codes, 25));
    free_codes(codes);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    char **codes = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(complexities_sum(codes, 2), 126384);
    free_codes(codes);
}

char **read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    char **codes = malloc(CODES * sizeof(char *));

    size_t i = 0;
    while (fgets(buffer, MAX_LINE, file)) {
        codes[i] = malloc(CODE_LENGTH * sizeof(char));
        memcpy(codes[i], buffer, CODE_LENGTH);
        ++i;
    }

    fclose(file);
    return codes;
}

void free_codes(char **codes) {
    for (size_t i = 0; i < CODES; i++) {
        free(codes[i]);
    }
    free(codes);
}

void free_presses(char **presses) {
    for (size_t i = 0; i < arrlenu(presses); ++i) {
        free(presses[i]);
    }
    arrfree(presses);
}

Position keypad_position(char button, bool numeric) {
    size_t start = numeric ? 0 : 4;
    size_t stop = numeric ? 4 : 6;

    for (size_t i = start; i < stop; ++i) {
        char *match = strchr(keypads[i], button);
        if (match) {
            return (Position){match - keypads[i], i};
        }
    }

    return (Position){-1, -1};
}

// Creates a sequence of button presses for given buttons and counts.
char *button_presses(char buttons[3], size_t counts[3]) {
    size_t count = counts[0] + counts[1] + counts[2];
    char *presses = calloc(count + 1, sizeof(char));

    size_t offset = 0;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < counts[i]; ++j) {
            presses[offset + j] = buttons[i];
        }
        offset += counts[i];
    }

    return presses;
}

// Creates a list of all possible button press sequences that cause the robot
// to press the button, starting from the previous button.
char **create_presses(char previous, char button) {
    bool numeric = isdigit(previous) || isdigit(button);
    Position prev_pos = keypad_position(previous, numeric);
    Position button_pos = keypad_position(button, numeric);
    int dx = button_pos.x - prev_pos.x;
    int dy = button_pos.y - prev_pos.y;

    char **presses = 0;
    if (dy && !dx) {
        // Move only vertically
        char *buttons = button_presses(
            (char[3]){dy > 0 ? 'v' : '^', 'A', ' '}, (size_t[3]){abs(dy), 1, 0}
        );
        arrput(presses, buttons);
    } else if (dx && !dy) {
        // Move only horizontally
        char *buttons = button_presses(
            (char[3]){dx > 0 ? '>' : '<', 'A', ' '}, (size_t[3]){abs(dx), 1, 0}
        );
        arrput(presses, buttons);
    } else {
        // Move vertically first (repeated presses allow for shorter sequences)
        if (keypads[button_pos.y + (numeric ? 0 : 4)][prev_pos.x] != ' ') {
            char *buttons = button_presses(
                (char[3]){dy > 0 ? 'v' : '^', dx > 0 ? '>' : '<', 'A'},
                (size_t[3]){abs(dy), abs(dx), 1}
            );
            arrput(presses, buttons);
        }

        // Move horizontally first
        if (keypads[prev_pos.y + (numeric ? 0 : 4)][button_pos.x] != ' ') {
            char *buttons = button_presses(
                (char[3]){dx > 0 ? '>' : '<', dy > 0 ? 'v' : '^', 'A'},
                (size_t[3]){abs(dx), abs(dy), 1}
            );
            arrput(presses, buttons);
        }
    }

    return presses;
}

// Encodes the buttons and level for the memoization table.
size_t memo_index(char previous, char button, size_t level) {
    char *chars = "^<v>A0123456789";
    char *p = strchr(chars, previous);
    char *b = strchr(chars, button);
    return level * 10000 + (p - chars) * 100 + (b - chars);
}

// Returns the minimum number of keypresses on the keypads-th directional
// keypad needed to press the button on the level-th keypad, starting from the
// previous button.
size_t min_presses(
    char previous, char button, size_t level, size_t keypads, Memory **memo
) {
    size_t index = memo_index(previous, button, level);
    if (hmgeti(*memo, index) != -1) {
        return hmget(*memo, index);
    }

    size_t min = SIZE_MAX;
    char **presses = create_presses(previous, button);
    for (size_t i = 0; i < arrlenu(presses); ++i) {
        char *sequence = presses[i];

        size_t length = 0;
        if (level == keypads) {
            length = strlen(sequence);
        } else {
            for (size_t j = 0; sequence[j]; ++j) {
                char previous = j ? sequence[j - 1] : 'A';
                length += min_presses(
                    previous, sequence[j], level + 1, keypads, memo
                );
            }
        }

        if (length < min) {
            min = length;
        }
    }

    free_presses(presses);
    hmput(*memo, index, min);
    return min;
}

size_t code_value(char code[CODE_LENGTH]) {
    return (code[0] - '0') * 100 + (code[1] - '0') * 10 + (code[2] - '0');
}

// Returns the sum of complexities of all codes. Each code value is multiplied
// by the number of keypresses needed on the keypads-th directional keypad.
size_t complexities_sum(char **codes, size_t keypads) {
    size_t complexities = 0;
    Memory *memo = 0;

    for (size_t i = 0; i < CODES; ++i) {
        char *code = codes[i];
        size_t length = 0;

        for (size_t j = 0; j < CODE_LENGTH; ++j) {
            char previous = j ? code[j - 1] : 'A';
            length += min_presses(previous, code[j], 0, keypads, &memo);
        }

        complexities += length * code_value(code);
    }

    hmfree(memo);
    return complexities;
}
