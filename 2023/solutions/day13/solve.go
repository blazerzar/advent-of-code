package day13

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
)

const day = 13

type pattern []string
type numericalPattern struct {
	rows    []uint
	columns []uint
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	patterns := readInput(inputPath)
	fmt.Println(summarizeNotes(patterns, 0))
	fmt.Println(summarizeNotes(patterns, 1))
}

func test(testInputPath string) {
	patterns := readInput(testInputPath)
	solutions.AssertEq(summarizeNotes(patterns, 0), 405)
	solutions.AssertEq(summarizeNotes(patterns, 1), 400)
}

func readInput(path string) []pattern {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	patterns := []pattern{[]string{}}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			patterns = append(patterns, []string{})
		} else {
			patterns[len(patterns)-1] = append(patterns[len(patterns)-1], line)
		}
	}

	return patterns
}

// Treat each row and column as a binary number, where '#' is 1 and '.' is 0.
func patternToNumbers(p pattern) numericalPattern {
	rows := make([]uint, len(p))
	columns := make([]uint, len(p[0]))

	for i, row := range p {
		for j, c := range []byte(row) {
			rows[i] *= 2
			columns[j] *= 2

			if c == '#' {
				rows[i]++
				columns[j]++
			}
		}
	}

	return numericalPattern{rows, columns}
}

// Return the index i, where the symmetry line is between rows i-1 and i.
func findSymmetry(numbers []uint, smudges int) int {
	for i := 1; i < len(numbers); i++ {
		size := i
		if len(numbers)-i < size {
			size = len(numbers) - i
		}

		mistakes := 0
		binaryMistakes := 0
		for j := 0; j < size; j++ {
			diff := numbers[i-1-j] ^ numbers[i+j]
			if diff != 0 {
				mistakes++
			}
			// If we allow for a smudge, the difference has to be a power of 2
			if diff != 0 && (diff&(diff-1)) == 0 {
				binaryMistakes++
			}
		}

		if mistakes == smudges && binaryMistakes == smudges {
			return i
		}
	}
	// For no symmetry, return 0, to not affect the sum
	return 0
}

func summarizeNotes(patterns []pattern, smudges int) int {
	result := 0
	for _, p := range patterns {
		numbers := patternToNumbers(p)
		rowSymmetry := findSymmetry(numbers.rows, smudges)
		columnSymmetry := findSymmetry(numbers.columns, smudges)
		result += 100*rowSymmetry + columnSymmetry
	}
	return result
}
