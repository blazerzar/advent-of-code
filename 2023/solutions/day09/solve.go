package day09

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

const day = 9

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	histories := readInput(inputPath)
	fmt.Println(extrapolateForward(histories))
	fmt.Println(extrapolateBackward(histories))
}

func test(testInputPath string) {
	testHistories := readInput(testInputPath)
	solutions.AssertEq(extrapolateForward(testHistories), 114)
	solutions.AssertEq(extrapolateBackward(testHistories), 2)
}

func readInput(path string) [][]int {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	histories := [][]int{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		numbers := strings.Fields(scanner.Text())
		history := make([]int, len(numbers))
		for i, number := range numbers {
			n, _ := strconv.Atoi(number)
			history[i] = n
		}

		histories = append(histories, history)
	}

	return histories
}

// Sum of forward predictions for each history.
func extrapolateForward(histories [][]int) int {
	sum := 0
	for _, history := range histories {
		sum += PredictNext(history)
	}

	return sum
}

// Sum of backward predictions for each history.
func extrapolateBackward(histories [][]int) int {
	sum := 0
	for _, history := range histories {
		sum += predictPrev(history)
	}

	return sum
}

func diff(numbers []int) []int {
	diffs := make([]int, len(numbers)-1)
	for i := 1; i < len(numbers); i++ {
		diffs[i-1] = numbers[i] - numbers[i-1]
	}

	return diffs
}

func allZero(numbers []int) bool {
	for _, number := range numbers {
		if number != 0 {
			return false
		}
	}

	return true
}

// PredictNext predicts the next value in the history using the differences
// between the observed values.
func PredictNext(history []int) int {
	diffs := diff(history)

	if allZero(diffs) {
		return history[len(history)-1]
	}

	return history[len(history)-1] + PredictNext(diffs)
}

// Predict previous value in the history analogous to predictNext.
func predictPrev(history []int) int {
	// Reverse slice and predict next value
	for i := 0; i < len(history)/2; i++ {
		j := len(history) - i - 1
		history[i], history[j] = history[j], history[i]
	}

	return PredictNext(history)
}
