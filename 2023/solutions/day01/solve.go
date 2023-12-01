package day01

import (
	"aoc-2023/solutions"
	"fmt"
	"os"
	"strings"
)

const day = 1

// Number zero is not possible, added just to make indexing easier
var numbers = []string{
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath1 := fmt.Sprintf("inputs/day_%02d_test_1.txt", day)
	testInputPath2 := fmt.Sprintf("inputs/day_%02d_test_2.txt", day)
	test(testInputPath1, testInputPath2)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	input := readInput(inputPath)
	fmt.Println(sumCalibrationValues(input, calibrationValuePart1))
	fmt.Println(sumCalibrationValues(input, calibrationValuePart2))
}

func test(testInputPath1 string, testInputPath2 string) {
	solutions.AssertEq(
		sumCalibrationValues(readInput(testInputPath1), calibrationValuePart1),
		142)
	solutions.AssertEq(
		sumCalibrationValues(readInput(testInputPath2), calibrationValuePart2),
		281)
}

func readInput(path string) []string {
	dat, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	return strings.Split(string(dat), "\n")
}

func sumCalibrationValues(lines []string, calibrationValue func(string) int) int {
	calibrationValues := 0

	for _, line := range lines {
		calibrationValues += calibrationValue(line)
	}

	return calibrationValues
}

// Create two digit integer using first and last number in the given line.
func calibrationValuePart1(line string) int {
	calibrationValue := 0

	for _, c := range line {
		if c < '0' || c > '9' {
			continue
		}

		calibrationValue = updateValue(calibrationValue, int(c-'0'))
	}

	return calibrationValue
}

// Create two digit integer using first and last number in the given line,
// where words can also be used to represent numbers.
func calibrationValuePart2(line string) int {
	calibrationValue := 0

	for i, c := range line {
		// Check if valid number word is at index i
		for j, number := range numbers {
			if i+len(number) > len(line) {
				continue
			}

			if line[i:i+len(number)] == number {
				calibrationValue = updateValue(calibrationValue, j)
				break
			}
		}

		if c >= '0' && c <= '9' {
			calibrationValue = updateValue(calibrationValue, int(c-'0'))
		}
	}

	return calibrationValue
}

// If calibration value is zero, update both digits, otherwise just the ones.
func updateValue(calibrationValue int, x int) int {
	if calibrationValue == 0 {
		return x*10 + x
	}
	return calibrationValue + x - calibrationValue%10
}
