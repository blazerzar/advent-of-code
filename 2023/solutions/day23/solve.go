package day23

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
)

const day = 23

type hike struct {
	x, y, steps int
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	trails := readInput(inputPath)
	fmt.Println(longestPath(trails, hike{x: 1, y: 0}, true))
	fmt.Println(longestPath(trails, hike{x: 1, y: 0}, false))
}

func test(testInputPath string) {
	trails := readInput(testInputPath)
	solutions.AssertEq(longestPath(trails, hike{x: 1, y: 0}, true), 94)
	solutions.AssertEq(longestPath(trails, hike{x: 1, y: 0}, false), 154)
}

func readInput(path string) [][]byte {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	var trails [][]byte
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		trails = append(trails, []byte(scanner.Text()))
	}

	return trails
}

var neighborsX = []int{0, 1, 0, -1}
var neighborsY = []int{-1, 0, 1, 0}

func longestPath(trails [][]byte, h hike, slippery bool) int {
	if h.y == len(trails)-1 {
		return h.steps
	}

	var longest int
	for n := range neighborsX {
		dx, dy := neighborsX[n], neighborsY[n]
		x, y := h.x+dx, h.y+dy
		if y < 0 || y >= len(trails) || x < 0 || x >= len(trails[y]) {
			continue
		}

		valid := true
		switch trails[y][x] {
		case '#':
			valid = false
		case '<':
			valid = valid && (!slippery || dx == -1)
		case '>':
			valid = valid && (!slippery || dx == 1)
		case '^':
			valid = valid && (!slippery || dy == -1)
		case 'v':
			valid = valid && (!slippery || dy == 1)
		}
		if !valid {
			continue
		}

		c := trails[y][x]
		trails[y][x] = '#'
		res := longestPath(trails, hike{x, y, h.steps + 1}, slippery)
		longest = max(longest, res)
		trails[y][x] = c
	}

	return longest
}
