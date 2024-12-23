#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 8
#define DAY 23

typedef struct Connection {
    char *key;
    bool value;
} Connection;

typedef struct Computer {
    char *key;
    Connection *value;
} Computer;

void test();
Computer *read_input(const char *filename);
void free_computers(Computer *computers);
size_t connected_computers(Computer *computers);
char *party_password(Computer *computers);

int main(void) {
    test();

    Computer *computers = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", connected_computers(computers));
    char *password = party_password(computers);
    printf("%s\n", password);
    free(password);
    free_computers(computers);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Computer *computers = read_input("inputs/day_" STR(DAY) "_test.txt");
    assert_eq(connected_computers(computers), 7);
    char *password = party_password(computers);
    assert_eq_s(password, "co,de,ka,ta");
    free(password);
    free_computers(computers);
}

int cmp_computers(const void *a, const void *b) {
    return strcmp(((Computer *) a)->key, ((Computer *) b)->key);
}

void add_connection(Computer **computers, char a[3], char b[3]) {
    Connection *connection_a = shget(*computers, a);
    if (!connection_a) {
        sh_new_strdup(connection_a);
    }
    Connection *connection_b = shget(*computers, b);
    if (!connection_b) {
        sh_new_strdup(connection_b);
    }

    shput(connection_a, b, true);
    shput(connection_b, a, true);
    shput(*computers, a, connection_a);
    shput(*computers, b, connection_b);
}

Computer *read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LINE];
    Computer *computers = 0;
    sh_new_strdup(computers);
    while (fgets(buffer, MAX_LINE, file)) {
        *(strchr(buffer, '\n')) = '\0';
        *(strchr(buffer, '-')) = '\0';
        add_connection(&computers, buffer, buffer + 3);
    }

    qsort(computers, shlenu(computers), sizeof(*computers), cmp_computers);

    fclose(file);
    return computers;
}

void free_computers(Computer *computers) {
    for (size_t i = 0; i < shlenu(computers); ++i) {
        shfree(computers[i].value);
    }
    shfree(computers);
}

// Returns true if the computer at index is connected to all computers with
// indices in the group.
bool connected_to_all(Computer *computers, size_t *group, size_t computer) {
    for (size_t i = 0; i < arrlenu(group); ++i) {
        if (!shget(computers[computer].value, computers[group[i]].key)) {
            return false;
        }
    }
    return true;
}

// Returns indices of all computers that are connected to all computers with
// indices in the given clique.
size_t *grow_clique(Computer *computers, size_t *clique) {
    size_t *connected = 0;
    size_t last_computer = clique[arrlenu(clique) - 1];

    for (size_t i = last_computer + 1; i < shlenu(computers); ++i) {
        if (connected_to_all(computers, clique, i)) {
            arrput(connected, i);
        }
    }

    return connected;
}

// Duplicate the given clique and add a computer to it.
size_t *duplicate_clique(size_t *arr, size_t computer) {
    size_t *dup = 0;
    for (size_t i = 0; i < arrlenu(arr); ++i) {
        arrput(dup, arr[i]);
    }
    arrput(dup, computer);
    return dup;
}

void free_cliques(size_t **cliques) {
    for (size_t i = 0; i < arrlenu(cliques); ++i) {
        arrfree(cliques[i]);
    }
    arrfree(cliques);
}

// Returns the list of cliques of the given size. If the size is not reached,
// the list of the largest cliques is returned.
size_t **get_cliques(Computer *computers, size_t size) {
    size_t **cliques = 0;
    for (size_t i = 0; i < shlenu(computers); ++i) {
        size_t *clique = 0;
        arrput(clique, i);
        arrput(cliques, clique);
    }

    while (arrlenu(cliques[0]) < size) {
        size_t **larger_cliques = 0;
        for (size_t i = 0; i < arrlenu(cliques); ++i) {
            size_t *clique = cliques[i];
            size_t *connected = grow_clique(computers, clique);
            for (size_t j = 0; j < arrlenu(connected); ++j) {
                arrput(larger_cliques, duplicate_clique(clique, connected[j]));
            }
            arrfree(connected);
        }

        if (!arrlenu(larger_cliques)) {
            return cliques;
        }

        free_cliques(cliques);
        cliques = larger_cliques;
    }

    return cliques;
}

// Returns the number of inter-connection groups of size 3.
size_t connected_computers(Computer *computers) {
    size_t **cliques = get_cliques(computers, 3);

    // Count groups with at least one suitable computer
    size_t groups = 0;
    for (size_t i = 0; i < arrlenu(cliques); ++i) {
        size_t *clique = cliques[i];
        groups += computers[clique[0]].key[0] == 't'
                  || computers[clique[1]].key[0] == 't'
                  || computers[clique[2]].key[0] == 't';
    }

    free_cliques(cliques);
    return groups;
}

// Returns the password for the LAN party, which is obtained by joining
// alphabetically sorted computers in the largest clique using a comma.
char *party_password(Computer *computers) {
    size_t **cliques = get_cliques(computers, SIZE_MAX);
    size_t *clique = cliques[0];

    char *password = calloc(3 * arrlenu(clique), sizeof(char));
    for (size_t i = 0; i < arrlenu(clique); ++i) {
        if (i) {
            password[3 * i - 1] = ',';
        }
        strcpy(password + 3 * i, computers[clique[i]].key);
    }

    free_cliques(cliques);
    return password;
}
