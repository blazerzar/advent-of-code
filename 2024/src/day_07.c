#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 128
#define DAY 07

typedef struct Equation {
    size_t test_value;
    size_t *numbers;
    size_t size;
} Equation;

void test();
Equation *read_input(const char *filename);
void free_equations(Equation *equations);
size_t total_calibration_result(Equation *equations, bool concat);

int main(void) {
    test();

    Equation *equation = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", total_calibration_result(equation, false));
    printf("%zu\n", total_calibration_result(equation, true));
    free_equations(equation);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Equation *equations = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(total_calibration_result(equations, false), 3749);
    assert_eq(total_calibration_result(equations, true), 11387);
    free_equations(equations);
}

Equation *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Equation *equations = 0;
    while (fgets(buffer, MAX_LINE, file)) {
        Equation equation = {0};
        char *numbers = strchr(buffer, ':') + 2;

        *(numbers - 2) = '\0';
        equation.test_value = strtoull(buffer, 0, 10);

        for (char *tok = strtok(numbers, " "); tok; tok = strtok(0, " ")) {
            arrput(equation.numbers, strtoull(tok, 0, 10));
            ++equation.size;
        }

        arrput(equations, equation);
    }

    fclose(file);
    return equations;
}

void free_equations(Equation *equations) {
    size_t size = arrlen(equations);
    for (size_t i = 0; i < size; ++i) {
        arrfree(equations[i].numbers);
    }
    arrfree(equations);
}

// Returns the number of possible configurations of operators that can make
// the equation true. The running sum is saved in the acc parameter. The i
// parameter is the index of the current number. If concat is true, we also
// try using the concatenation operator.
size_t true_equation(Equation equation, size_t acc, size_t i, bool concat) {
    if (i == equation.size) {
        return acc == equation.test_value;
    }

    size_t configurations = 0;

    if (concat) {
        size_t digits = log10(equation.numbers[i]) + 1;
        size_t concatenated = acc * pow(10, digits) + equation.numbers[i];
        configurations += true_equation(equation, concatenated, i + 1, concat);
    }

    // Add the current number or ...
    size_t added = acc + equation.numbers[i];
    configurations += true_equation(equation, added, i + 1, concat);

    // ... multiply by the current number
    size_t multiplied = (acc ? acc : 1) * equation.numbers[i];
    configurations += true_equation(equation, multiplied, i + 1, concat);

    return configurations;
}

// Returns the sum of test values from the equations that could be true.
// If concat is true, we also try using the concatenation operator.
size_t total_calibration_result(Equation *equations, bool concat) {
    size_t result = 0;
    size_t size = arrlen(equations);

    for (size_t i = 0; i < size; ++i) {
        if (true_equation(equations[i], 0, 0, concat) > 0) {
            result += equations[i].test_value;
        }
    }

    return result;
}
