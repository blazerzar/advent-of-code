#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <regex.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 4096
#define DAY 03

typedef struct Memory {
    const char **lines;
} Memory;

void test();
Memory read_input(const char *filename);
void free_memory(Memory memory);
int find_instructions(Memory memory, bool conditionals);

int main(void) {
    test();

    Memory memory = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%d\n", find_instructions(memory, false));
    printf("%d\n", find_instructions(memory, true));
    free_memory(memory);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Memory memory = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    assert_eq(find_instructions(memory, false), 161);
    free_memory(memory);

    memory = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    assert_eq(find_instructions(memory, true), 48);
    free_memory(memory);
}

Memory read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Memory memory = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        // Remove the newline character
        char *new_line_char = strchr(buffer, '\n');
        if (new_line_char) {
            *new_line_char = '\0';
        }

        char *line = malloc(strlen(buffer) + 1);
        strcpy(line, buffer);
        arrput(memory.lines, line);
    }

    fclose(file);
    return memory;
}

void free_memory(Memory memory) {
    size_t lines = arrlen(memory.lines);
    for (size_t i = 0; i < lines; ++i) {
        free((void *) memory.lines[i]);
    }
    arrfree(memory.lines);
}

// Find mul instructions and return the sum of their return values. If
// conditionals are enabled, the instructions do() and don't() enable and
// disable the mul instructions.
int find_instructions(Memory memory, bool conditionals) {
    int sum = 0;
    bool enabled = true;

    regex_t regex;
    regmatch_t match[1];
    regcomp(
        &regex, "mul\\([0-9]{1,3},[0-9]{1,3}\\)|do\\(\\)|don't\\(\\)",
        REG_EXTENDED
    );

    size_t lines = arrlen(memory.lines);
    for (size_t i = 0; i < lines; ++i) {
        char *line = malloc(strlen(memory.lines[i]) + 1);
        strcpy(line, memory.lines[i]);

        char *start = line;
        while (!regexec(&regex, start, 1, match, 0)) {
            // Replace the last ) with a null terminator
            start[match[0].rm_eo - 1] = '\0';

            if (!strcmp(start + match[0].rm_so, "do(")) {
                enabled = true;
            } else if (!strcmp(start + match[0].rm_so, "don't(")) {
                enabled = false;
            } else if (enabled || !conditionals) {
                // Create two strings for the two numbers
                char *left = start + match[0].rm_so + sizeof("mul");
                char *right = strchr(left, ',') + 1;
                right[-1] = '\0';
                sum += atoi(left) * atoi(right);
            }

            start += match[0].rm_eo;
        }

        free(line);
    }

    regfree(&regex);
    return sum;
}
