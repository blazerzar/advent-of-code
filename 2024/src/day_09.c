#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 20000
#define DAY 09

void test();
char *read_input(const char *filename);
size_t compact_blocks(const char *memory_map);
size_t compact_files(const char *memory_map);

int main(void) {
    test();

    const char *memory_map = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", compact_blocks(memory_map));
    printf("%zu\n", compact_files(memory_map));
    free((void *) memory_map);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    const char *memory_map = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(compact_blocks(memory_map), 1928);
    assert_eq(compact_files(memory_map), 2858);
    free((void *) memory_map);
}

char *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char *memory_map = malloc(MAX_LINE);
    fgets(memory_map, MAX_LINE, file);
    char *line_end = strchr(memory_map, '\n');
    if (line_end) {
        *line_end = '\0';
    }

    fclose(file);
    return memory_map;
}

// Computes the checksum after moving each file block to the leftmost free
// space, starting from the rightmost block.
size_t compact_blocks(const char *memory_map) {
    size_t checksum = 0;

    // We compute checksum for file blocks at map_front
    size_t map_front = 0;
    size_t block_next = 0;
    size_t block_end = memory_map[map_front] - '0';

    // File blocks at map_back get moved to free blocks at map_front
    size_t map_back = strlen(memory_map) - 1;
    size_t file_size = memory_map[map_back] - '0';

    while (map_front < map_back) {
        bool free_block = map_front % 2;
        size_t file_id = (free_block ? map_back : map_front) / 2;
        checksum += block_next * file_id;

        // Move to the next block and find non-empty section if needed
        ++block_next;
        while (block_next == block_end) {
            ++map_front;
            block_end += memory_map[map_front] - '0';
        }

        // We copy a single block from the file, when done, move to the next
        file_size -= free_block;
        if (!file_size) {
            map_back -= 2;
            file_size = memory_map[map_back] - '0';
        }
    }

    // There may be some file blocks left if the last free space was too small
    while (file_size) {
        size_t file_id = map_back / 2;
        checksum += block_next * file_id;
        ++block_next;
        --file_size;
    }

    return checksum;
}

typedef struct Space {
    size_t start, end, file_id;
} Space;

Space *create_memory_list(const char *memory_map) {
    size_t len = strlen(memory_map);
    Space *spaces = 0;
    arrsetlen(spaces, len);

    size_t start = 0;
    for (size_t i = 0; i < len; ++i) {
        // Mark empty space with -1 = SIZE_MAX
        size_t file_id = i % 2 ? -1 : i / 2;
        size_t size = memory_map[i] - '0';
        spaces[i] = (Space){start, start + size, file_id};
        start += size;
    }

    return spaces;
}

// Computes the checksum by summing the products of the file ID and the block
// position for each file.
size_t compute_checksum(Space *spaces) {
    size_t checksum = 0;
    size_t len = arrlen(spaces);

    for (size_t i = 0; i < len; ++i) {
        if (spaces[i].file_id == (size_t) -1) {
            continue;
        }
        for (size_t j = spaces[i].start; j < spaces[i].end; ++j) {
            checksum += spaces[i].file_id * j;
        }
    }

    return checksum;
}

size_t find_file(Space *spaces, size_t file_id) {
    size_t len = arrlen(spaces);
    for (size_t i = 0; i < len; ++i) {
        if (spaces[i].file_id == file_id) {
            return i;
        }
    }
    return len;
}

size_t find_free_space(Space *spaces, size_t size) {
    size_t len = arrlen(spaces);
    for (size_t i = 0; i < len; ++i) {
        if (spaces[i].file_id != (size_t) -1) {
            continue;
        }
        if (spaces[i].end - spaces[i].start >= size) {
            return i;
        }
    }
    return len;
}

// Computes the checksum after moving each file to the leftmost free space
// which is large enough. We try moving files from the largest file ID to the
// smallest.
size_t compact_files(const char *memory_map) {
    size_t len = strlen(memory_map);
    Space *spaces = create_memory_list(memory_map);

    for (size_t file_id = len / 2; file_id > 0; --file_id) {
        size_t i = find_file(spaces, file_id);
        size_t size = spaces[i].end - spaces[i].start;
        size_t j = find_free_space(spaces, size);
        if (j >= i) {
            continue;
        }
        size_t free_size = spaces[j].end - spaces[j].start;

        // Move file to free space and adjust its size
        spaces[j].file_id = spaces[i].file_id;
        spaces[j].end = spaces[j].start + size;

        // Mark old space as empty and merge adjacent empty spaces
        spaces[i].file_id = -1;
        if (i + 1 < len && spaces[i + 1].file_id == (size_t) -1) {
            spaces[i + 1].start = spaces[i].start;
            arrdel(spaces, i);
            --len;
        }
        if (spaces[i - 1].file_id == (size_t) -1) {
            spaces[i - 1].end = spaces[i].end;
            arrdel(spaces, i);
            --len;
        }

        if (size == free_size) {
            continue;
        }

        // Add new empty space if there is any free space left
        if (j + 1 < len && spaces[j + 1].file_id == (size_t) -1) {
            spaces[j + 1].start = spaces[j].end;
        } else {
            Space s = {spaces[j].end, spaces[j].end + free_size - size, -1};
            arrins(spaces, j + 1, s);
            ++len;
        }
    }

    size_t checksum = compute_checksum(spaces);

    arrfree(spaces);
    return checksum;
}
