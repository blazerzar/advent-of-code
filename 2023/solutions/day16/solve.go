package day16

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
)

const day = 16

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	layout := readInput(inputPath)
	fmt.Println(energized(layout, beam{0, 0, right}))
	fmt.Println(maxEnergy(layout))
}

func test(testInputPath string) {
	layout := readInput(testInputPath)
	solutions.AssertEq(energized(layout, beam{0, 0, right}), 46)
	solutions.AssertEq(maxEnergy(layout), 51)
}

func readInput(path string) []string {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	lines := []string{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		lines = append(lines, scanner.Text())
	}

	return lines
}

const (
	up         = iota
	right      = iota
	down       = iota
	left       = iota
	vertical   = iota
	horizontal = iota
)

var movesX = []int{0, 1, 0, -1}
var movesY = []int{-1, 0, 1, 0}
var bounces = map[byte][]int{
	'/':  {right, up, left, down},
	'\\': {left, down, right, up},
	'|':  {up, vertical, down, vertical},
	'-':  {horizontal, right, horizontal, left},
}

type beam struct {
	x, y, direction int
}

// Find the number of energized cells given a starting beam configuration.
func energized(layout []string, start beam) uint {
	var queue = []beam{start}
	visited := make([][]uint8, len(layout))
	for i := range layout {
		visited[i] = make([]uint8, len(layout[i]))
	}

	for len(queue) != 0 {
		current := queue[0]
		queue = queue[1:]
		x, y, direction := current.x, current.y, current.direction
		if visited[y][x] == 1<<direction {
			continue
		}
		visited[y][x] |= 1 << direction
		c := layout[y][x]

		toUpdate := []beam{current}
		// On non-empty spaces, we first need to update the direction
		if c != '.' {
			toUpdate[0].direction = bounces[c][direction]
			if toUpdate[0].direction > left {
				// Split vertical or horizontal beam into two
				toUpdate = append(toUpdate, toUpdate[0])
				toUpdate[0].direction -= 2
				toUpdate[1].direction -= 4
			}
		}
		queue = append(queue, moveForward(layout, toUpdate)...)
	}

	return countEnergized(visited)
}

func moveForward(layout []string, beams []beam) []beam {
	var updated []beam
	for _, b := range beams {
		x := b.x + movesX[b.direction]
		y := b.y + movesY[b.direction]
		if y >= 0 && y < len(layout) && x >= 0 && x < len(layout[y]) {
			updated = append(updated, beam{x, y, b.direction})
		}
	}
	return updated
}

func countEnergized(visited [][]uint8) uint {
	var energized uint
	for _, row := range visited {
		for _, cell := range row {
			if cell != 0 {
				energized++
			}
		}
	}
	return energized
}

// Find the starting beam configuration that maximizes the energy.
func maxEnergy(layout []string) uint {
	maximum := uint(0)

	for y := range layout {
		for x := range []int{0, len(layout[y]) - 1} {
			if e := energized(layout, beam{x, y, right}); e > maximum {
				maximum = e
			}
		}
	}
	for x := range layout[0] {
		for y := range []int{0, len(layout) - 1} {
			if e := energized(layout, beam{x, y, down}); e > maximum {
				maximum = e
			}
		}
	}

	return maximum
}
