#pragma once

#define STR_(x) #x
#define STR(x) STR_(x)

#define assert_eq(expected, got) _Generic((expected), \
    int: assert_eq_i, \
    long: assert_eq_l, \
    long long: assert_eq_ll, \
    size_t: assert_eq_z \
)(expected, got)

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
