package solutions

import (
	"fmt"
	"os"
)

func AssertEq[T comparable](a T, b T) {
	if a != b {
		fmt.Printf("Assertion failed: %v != %v\n", a, b)
		os.Exit(1)
	}
}
