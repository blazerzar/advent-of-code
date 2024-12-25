#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#include "utils.h"

#define MAX_LINE 32
#define DAY 24

typedef enum Operation { AND, OR, XOR } Operation;

typedef struct Wire {
    size_t key;
    bool value;
} Wire;

typedef struct Connection {
    size_t key;
    Operation op;
    char out[4];
    char lhs[4];
    char rhs[4];
} Connection;

typedef struct Device {
    Wire *wires;
    Connection *connections;
    size_t out_wires;
} Device;

void test();
Device read_input(const char *filename);
void free_device(Device *device);
size_t system_output(Device *device);
char *swapped_gates(Device *device);

int main(void) {
    test();

    Device device = read_input("inputs/day_" STR(DAY) ".txt");
    printf("%zu\n", system_output(&device));
    char *swaps = swapped_gates(&device);
    printf("%s\n", swaps);
    free(swaps);
    free_device(&device);

    return EXIT_SUCCESS;
}

// Tests for the included examples.
void test() {
    Device device = read_input("inputs/day_" STR(DAY) "_test_1.txt");
    assert_eq(system_output(&device), 4);
    free_device(&device);

    device = read_input("inputs/day_" STR(DAY) "_test_2.txt");
    assert_eq(system_output(&device), 2024);
    free_device(&device);
}

size_t static inline wire_key(char wire[3]) {
    return (wire[0] * 1000 + wire[1]) * 1000 + wire[2];
}

Wire *parse_wires(FILE *file) {
    Wire *wires = 0;

    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file) && strlen(buffer) > 1) {
        char *value = strchr(buffer, ' ') + 1;
        hmput(wires, wire_key(buffer), atoi(value));
    }

    return wires;
}

Connection create_connection(char *op, char *out, char *lhs, char *rhs) {
    Connection connection = {
        .key = wire_key(out),
        .op = !strcmp(op, "AND") ? AND : (!strcmp(op, "OR") ? OR : XOR),
    };
    strcpy(connection.out, out);
    strcpy(connection.lhs, lhs);
    strcpy(connection.rhs, rhs);
    return connection;
}

Connection *parse_connections(FILE *file, Device *device) {
    Connection *connections = 0;

    char buffer[MAX_LINE];
    while (fgets(buffer, MAX_LINE, file)) {
        char *out = strchr(buffer, '>') + 2;
        char *op = buffer + 4;
        out[3] = '\0';

        char *rhs = strchr(op, ' ') + 1;
        rhs[3] = '\0';
        rhs[-1] = '\0';
        buffer[3] = '\0';

        Connection connection = create_connection(op, out, buffer, rhs);
        hmputs(connections, connection);
        device->out_wires += out[0] == 'z';
    }

    return connections;
}

Device read_input(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) {
        perror(filename);
        exit(EXIT_FAILURE);
    }

    Device device = {0};
    device.wires = parse_wires(file);
    device.connections = parse_connections(file, &device);

    fclose(file);
    return device;
}

void free_device(Device *device) {
    hmfree(device->wires);
    hmfree(device->connections);
}

bool static inline evaluate_wire(bool lhs, bool rhs, Operation op) {
    return op == AND ? lhs && rhs : op == OR ? lhs || rhs : lhs ^ rhs;
}

// Returns the value of the wire by reading the value from the hash map or
// recursively evaluating the wire's connections.
bool wire_value(Device *device, char *wire) {
    size_t key = wire_key(wire);
    if (hmgeti(device->wires, key) != -1) {
        return hmget(device->wires, key);
    }

    Connection connection = hmgets(device->connections, key);
    bool lhs = wire_value(device, connection.lhs);
    bool rhs = wire_value(device, connection.rhs);
    bool value = evaluate_wire(lhs, rhs, connection.op);

    hmput(device->wires, key, value);
    return value;
}

// Returns the output of the system by evaluating the output wires and joining
// them as bits into a single number.
size_t system_output(Device *device) {
    size_t output = 0;

    char wire[4] = {0};
    for (size_t i = device->out_wires - 1; i != SIZE_MAX; --i) {
        sprintf(wire, "z%02zu", i);
        output = output * 2 + wire_value(device, wire);
    }

    return output;
}

// Returns the output of the adder in device for the given inputs x and y.
size_t adder_output(Device *device, size_t x, size_t y) {
    hmfree(device->wires);
    device->wires = 0;

    char wire[4] = {0};
    for (size_t i = 0; i < device->out_wires; ++i) {
        sprintf(wire, "x%02zu", i);
        hmput(device->wires, wire_key(wire), (x >> i) & 1);
        sprintf(wire, "y%02zu", i);
        hmput(device->wires, wire_key(wire), (y >> i) & 1);
    }

    return system_output(device);
}

// Returns true if the adder in device is correct for the given bit.
bool adder_correct(Device *device, size_t bit) {
    for (size_t a = 0; a < 4; ++a) {
        size_t x = (size_t) ((a & 1) != 0) << bit;
        size_t y = (size_t) ((a & 2) != 0) << bit;
        if (x + y != adder_output(device, x, y)) {
            return false;
        }
    }
    return true;
}

// Swaps the connections of the two wires in the device.
void swap_wires(Device *device, char *wire_a, char *wire_b) {
    size_t key_a = wire_key(wire_a);
    size_t key_b = wire_key(wire_b);

    Connection connection_a = hmgets(device->connections, key_a);
    Connection connection_b = hmgets(device->connections, key_b);
    connection_a.key = key_b;
    connection_b.key = key_a;
    strcpy(connection_a.out, wire_b);
    strcpy(connection_b.out, wire_a);

    hmputs(device->connections, connection_a);
    hmputs(device->connections, connection_b);
}

// Returns true if there is a cycle in evaluating the wire in the device.
bool connection_cycle(Device *device, char *wire) {
    size_t *q = 0;
    arrput(q, wire_key(wire));
    bool cycle = false;

    while (arrlenu(q) && !cycle) {
        size_t current = q[0];
        arrdel(q, 0);

        if (hmgeti(device->connections, current) != -1) {
            Connection connection = hmgets(device->connections, current);
            arrput(q, wire_key(connection.lhs));
            arrput(q, wire_key(connection.rhs));
            cycle |= !strcmp(connection.lhs, wire);
            cycle |= !strcmp(connection.rhs, wire);
        }
    }

    arrfree(q);
    return cycle;
}

// Returns true if the adder in device is correct for the given bit after
// swapping the connections of the wires a and b. The connections are left
// swapped after the function returns.
bool adder_correct_swap(Device *device, size_t bit, char *a, char *b) {
    swap_wires(device, a, b);
    bool cycle = connection_cycle(device, a) || connection_cycle(device, b);
    bool correct = !cycle && adder_correct(device, bit);
    return correct;
}

// Marks all the wires connected to the given wire in the device, including
// the wire itself and excluding the input wires x and y.
Wire *connected_wires(Device *device, char wire[4], Wire *wires) {
    size_t key = wire_key(wire);
    if (wire[0] != 'x' && wire[0] != 'y') {
        hmput(wires, key, true);
    }

    if (hmgeti(device->connections, key) != -1) {
        Connection connection = hmgets(device->connections, key);
        wires = connected_wires(device, connection.lhs, wires);
        wires = connected_wires(device, connection.rhs, wires);
    }

    return wires;
}

// Returns true if the device performs correct addition for all bits higher
// or equal to the given bit. The wires list contains the wires that are
// already known to be correct. In the swaps list, we return the wires that
// need to be swapped.
bool swapped_wires(Device *device, size_t bit, Wire *wires, char ***swaps) {
    Wire *good_wires = 0;
    for (size_t i = 0; i < hmlenu(wires); ++i) {
        hmputs(good_wires, wires[i]);
    }

    for (size_t i = bit; i < device->out_wires - 1; ++i) {
        bool good = adder_correct(device, i);
        if (good) {
            char wire[4] = {0};
            sprintf(wire, "z%02zu", i);
            good_wires = connected_wires(device, wire, good_wires);
            continue;
        }

        char **swappable = 0;
        for (size_t j = 0; j < hmlenu(device->connections); ++j) {
            size_t key = device->connections[j].key;
            if (hmgeti(good_wires, key) == -1) {
                arrput(swappable, device->connections[j].out);
            }
        }

        for (size_t j = 0; j < arrlenu(swappable) && !good; ++j) {
            char *wire_a = swappable[j];
            for (size_t k = j + 1; k < arrlenu(swappable) && !good; ++k) {
                char *wire_b = swappable[k];
                if (!adder_correct_swap(device, i, wire_a, wire_b)) {
                    swap_wires(device, wire_a, wire_b);
                    continue;
                }

                // If the swap is good, we check the remaining bits recursively
                hmput(good_wires, wire_key(wire_a), true);
                hmput(good_wires, wire_key(wire_b), true);
                good = swapped_wires(device, i, good_wires, swaps);
                if (good) {
                    // DFS found a solution, add the swap to the list
                    arrput(*swaps, wire_a);
                    arrput(*swaps, wire_b);
                }
                hmdel(good_wires, wire_key(wire_a));
                hmdel(good_wires, wire_key(wire_b));
                swap_wires(device, wire_a, wire_b);
            }
        }

        arrfree(swappable);
        hmfree(good_wires);
        return good;
    }

    // If we reached the last bit, the adder is correct
    hmfree(good_wires);
    return true;
}

int cmp(const void *a, const void *b) {
    return strcmp(*(char **) a, *(char **) b);
}

char *swapped_gates(Device *device) {
    char **swaps = 0;
    assert(swapped_wires(device, 0, 0, &swaps));
    qsort(swaps, arrlenu(swaps), sizeof(swaps[0]), cmp);

    char *result = malloc(arrlenu(swaps) * 4 * sizeof(char));
    for (size_t i = 0; i < arrlenu(swaps); ++i) {
        if (i) {
            result[i * 4 - 1] = ',';
        }
        strcpy(result + i * 4, swaps[i]);
    }

    return result;
}
