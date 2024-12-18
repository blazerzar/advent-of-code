#pragma once

#define STR_(x) #x
#define STR(x) STR_(x)

#define assert_eq(a, b) _Generic((a), \
    int: assert_eq_i, \
    long: assert_eq_l, \
    long long: assert_eq_ll, \
    size_t: assert_eq_z, \
    char *: assert_eq_s \
)(a, b)

static inline void assert_eq_i(int a, int b) {
    if (a != b) {
        fprintf(stderr, "Assertion failed: %d != %d\n", a, b);
        abort();
    }
}

static inline void assert_eq_l(long a, long b) {
    if (a != b) {
        fprintf(stderr, "Assertion failed: %ld != %ld\n", a, b);
        abort();
    }
}

static inline void assert_eq_ll(long long a, long long b) {
    if (a != b) {
        fprintf(stderr, "Assertion failed: %lld != %lld\n", a, b);
        abort();
    }
}

static inline void assert_eq_z(size_t a, size_t b) {
    if (a != b) {
        fprintf(stderr, "Assertion failed: %zu != %zu\n", a, b);
        abort();
    }
}

static inline void assert_eq_s(char *a, char *b) {
    if (strcmp(a, b)) {
        fprintf(stderr, "Assertion failed: %s != %s\n", a, b);
        abort();
    }
}
