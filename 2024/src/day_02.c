#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 1024
#define MAX_REPORT 8
#define DAY 02

typedef struct Report {
    size_t length;
    int levels[MAX_REPORT];
} Report;

void test();
Report *read_input(const char *filename);
size_t safe_reports(Report *reports, bool dampener);

int main(void) {
    test();

    Report *reports = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", safe_reports(reports, false));
    printf("%zu\n", safe_reports(reports, true));
    arrfree(reports);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Report *reports = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(safe_reports(reports, false), 2);
    assert_eq(safe_reports(reports, true), 4);
    arrfree(reports);
}

Report parse_report(char *line) {
    Report report = {0};

    for (char *token = strtok(line, " "); token; token = strtok(0, " ")) {
        report.levels[report.length++] = atoi(token);
    }

    return report;
}

Report *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Report *reports = 0;
    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        arrput(reports, parse_report(buffer));
    }

    fclose(file);
    return reports;
}

// A report is considered safe if the difference between two consecutive
// levels is at most 3 and the levels are either all increasing or all
// decreasing.
bool is_safe(Report report, size_t skip) {
    assert(report.length > 2);

    int previous = report.levels[0];
    int sign = report.levels[1] - report.levels[0];
    size_t start = 1;

    if (skip == 0) {
        sign = report.levels[2] - report.levels[1];
        previous = report.levels[1];
        start = 2;
    } else if (skip == 1) {
        sign = report.levels[2] - report.levels[0];
    }

    for (size_t i = start; i < report.length; ++i) {
        if (i == skip) {
            continue;
        }

        int diff = report.levels[i] - previous;
        if (abs(diff) > 3 || diff * sign <= 0) {
            return false;
        }

        previous = report.levels[i];
    }

    return true;
}

size_t safe_reports(Report *reports, bool dampener) {
    size_t count = 0;
    size_t size = (size_t) arrlen(reports);

    for (size_t i = 0; i < size; i++) {
        bool safe = is_safe(reports[i], reports[i].length);
        if (dampener) {
            for (size_t j = 0; j < reports[i].length; j++) {
                safe |= is_safe(reports[i], j);
            }
        }
        count += safe;
    }
    return count;
}
