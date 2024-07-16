package day12

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

const day = 12

type row struct {
	record string
	groups []int
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	rows := readInput(inputPath)
	fmt.Println(possibleArrangements(rows))
	fmt.Println(possibleArrangements(unfoldRows(rows)))
}

func test(testInputPath string) {
	rows := readInput(testInputPath)
	solutions.AssertEq(possibleArrangements(rows), 21)
	solutions.AssertEq(possibleArrangements(unfoldRows(rows)), 525152)
}

func readInput(path string) []row {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	rows := []row{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.Split(scanner.Text(), " ")
		groups := []int{}
		for _, group := range strings.Split(line[1], ",") {
			g, _ := strconv.Atoi(group)
			groups = append(groups, g)
		}
		rows = append(rows, row{record: line[0], groups: groups})
	}

	return rows
}

func possibleArrangements(rows []row) int {
	arrangements := 0
	for _, r := range rows {
		arrangements += numArrangements(r)
	}
	return arrangements
}

func unfoldRows(rows []row) []row {
	unfolded := []row{}
	for _, r := range rows {
		unfoldedR := row{r.record, r.groups}
		for i := 0; i < 4; i++ {
			unfoldedR.record += "?" + r.record
			unfoldedR.groups = append(unfoldedR.groups, r.groups...)
		}
		unfolded = append(unfolded, unfoldedR)
	}
	return unfolded
}

func numArrangements(r row) int {
	var sb strings.Builder
	sb.WriteString(".")
	for _, g := range r.groups {
		sb.WriteString(strings.Repeat("#", g))
		sb.WriteString(".")
	}

	regex := sb.String()
	states := make([]int, len(regex))
	states[0] = 1

	for _, c := range []byte(r.record) {
		newStates := make([]int, len(states))
		if c == '.' || c == '?' {
			newStates[len(states)-1] += states[len(states)-1]
		}
		for i := 0; i < len(states)-1; i++ {
			// Stay in the same state on . and ?
			if regex[i] == '.' && (c == '.' || c == '?') {
				newStates[i] += states[i]
			}
			// Split state with ?, move on # and .
			if c == '?' || c == regex[i+1] {
				newStates[i+1] += states[i]
			}
		}
		states = newStates
	}

	return states[len(states)-2] + states[len(states)-1]
}
