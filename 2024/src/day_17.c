#include <stdio.h>
#include <stdlib.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 64
#define DAY 17

#define UNUSED(x) (void)(x)
#define MAX_PROGRAM 16

enum Register { RA, RB, RC, REGISTERS };
typedef struct Computer {
    size_t registers[REGISTERS];
    size_t program[MAX_PROGRAM];
    size_t instructions;
    size_t ip;
} Computer;

void test();
Computer read_input(const char *filename);
char *run_program(Computer computer);
size_t output_itself(Computer computer);

int main(void) {
    test();

    Computer computer = read_input("inputs/day_" STR(DAY) ".txt");
    char *output = run_program(computer);
    printf("%s\n", output);
    free(output);
    printf("%zu\n", output_itself(computer));

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Computer computer = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    char *output = run_program(computer);
    assert_eq(output, "4,6,3,5,6,3,5,2,1,0");
    free(output);

    computer = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    computer.registers[RA] = 117440;
    output = run_program(computer);
    assert_eq(output, "0,3,5,4,3,0");
    free(output);
    assert_eq(output_itself(computer), 117440);
}

Computer read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Computer computer = {0};
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        if (strstr(buffer, "Register")) {
            char reg = buffer[sizeof("Register")];
            size_t val = atoi(buffer + sizeof("Register A:"));
            computer.registers[reg - 'A'] = val;
        } else if (strlen(buffer) > 1) {
            char *program = buffer + strlen("Program: ");
            for (char *t = strtok(program, ","); t; t = strtok(0, ",")) {
                computer.program[computer.instructions++] = atoi(t);
            }
        }
    }

    fclose(file);
    return computer;
}

size_t combo_operand(size_t *registers, size_t operand) {
    if (operand <= REGISTERS) {
        return operand;
    } else {
        return registers[operand - REGISTERS - 1];
    }
}

size_t divide(size_t *registers, size_t operand) {
    size_t combo = combo_operand(registers, operand);
    return registers[RA] >> combo;
}

size_t adv(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    registers[RA] = divide(registers, operand);
    return ip + 2;
}

size_t bxl(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    registers[RB] ^= operand;
    return ip + 2;
}

size_t bst(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    registers[RB] = combo_operand(registers, operand) % 8;
    return ip + 2;
}

size_t jnz(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    return registers[RA] ? operand : (ip + 2);
}

size_t bxc(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(operand);
    UNUSED(io);
    registers[RB] ^= registers[RC];
    return ip + 2;
}

size_t out(size_t *registers, size_t operand, size_t ip, size_t **io) {
    arrput(*io, combo_operand(registers, operand) % 8);
    return ip + 2;
}

size_t bdv(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    registers[RB] = divide(registers, operand);
    return ip + 2;
}

size_t cdv(size_t *registers, size_t operand, size_t ip, size_t **io) {
    UNUSED(io);
    registers[RC] = divide(registers, operand);
    return ip + 2;
}

size_t (*instructions[8])(size_t *, size_t, size_t, size_t **) = {
    adv, bxl, bst, jnz, bxc, out, bdv, cdv,
};

// Runs the program and returns the output as a string.
char *run_program(Computer computer) {
    size_t ip = 0;
    size_t *io = 0;

    while (ip < computer.instructions) {
        size_t opcode = computer.program[ip];
        size_t operand = computer.program[ip + 1];
        ip = instructions[opcode](computer.registers, operand, ip, &io);
    }

    // Join the output using commas
    char *output = malloc(2 * arrlen(io));
    for (size_t i = 0; i < arrlenu(io); i++) {
        output[2 * i] = io[i] + '0';
        if (i + 1 < arrlenu(io)) {
            output[2 * i + 1] = ',';
        } else {
            output[2 * i + 1] = '\0';
        }
    }

    arrfree(io);
    return output;
}

// Runs a single loop of the program with the given value of register A and
// returns the printed value of register B.
size_t run_loop(Computer computer, size_t a) {
    computer.registers[RA] = a;
    computer.registers[RB] = 0;
    computer.registers[RC] = 0;
    size_t *io = 0;

    // We ignore jumps
    for (size_t i = 0; i < computer.instructions; i += 2) {
        size_t opcode = computer.program[i];
        size_t operand = computer.program[i + 1];
        instructions[opcode](computer.registers, operand, i, &io);
    }

    size_t output = io[0];
    arrfree(io);
    return output;
}

// Returns the smallest value of register A that causes the program to output
// the copy of itself. The solution assumes that the register A is divided by
// 8 in each iteration of the program, that the last instruction is a
// conditional jump to the first instruction and that each iteration prints
// a single value.
size_t output_itself(Computer computer) {
    size_t *a_candidates = 0;
    // At the end of the program, register A must be 0
    arrput(a_candidates, 0);

    // One number is printed in each program loop
    for (size_t i = computer.instructions; i > 0; --i) {
        size_t *new_candidates = 0;

        // Find values of register A that lead to printing the current output
        for (size_t j = 0; j < arrlenu(a_candidates); ++j) {
            size_t candidate = a_candidates[j];

            // Dividing the new value by 8 must lead to the current cadidate
            for (size_t a = 8 * candidate; a < 8 * (candidate + 1); ++a) {
                size_t output = run_loop(computer, a);
                // The printed value must match the program instruction
                if (output == computer.program[i - 1]) {
                    arrput(new_candidates, a);
                }
            }
        }

        arrfree(a_candidates);
        a_candidates = new_candidates;
    }

    size_t result = a_candidates[0];
    arrfree(a_candidates);
    return result;
}
