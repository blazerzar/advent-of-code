package day21

import (
	"aoc-2023/solutions"
	"aoc-2023/solutions/day09"
	"bufio"
	"fmt"
	"os"
)

const day = 21

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	garden := readInput(inputPath)
	fmt.Println(reach(garden, 64, false))
	fmt.Println(reachBig(garden, 26501365))
}

func test(testInputPath string) {
	garden := readInput(testInputPath)
	solutions.AssertEq(reach(garden, 6, false), 16)
	solutions.AssertEq(reach(garden, 50, true), 1594)
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

type plot struct {
	x, y int
}

var neighbors = [4][2]int{{0, -1}, {1, 0}, {0, 1}, {-1, 0}}

func update(garden []string, plots map[plot]bool, cycle bool) map[plot]bool {
	h, w := len(garden), len(garden[0])
	updated := map[plot]bool{}
	for p := range plots {
		for _, n := range neighbors {
			x, y := p.x+n[0], p.y+n[1]
			oob := y < 0 || y >= len(garden) || x < 0 || x >= len(garden[y])
			if !cycle && oob {
				continue
			}
			c := garden[(y%h+h)%h][(x%w+w)%w]
			if c == '#' {
				continue
			}

			updated[plot{x, y}] = true
		}
	}
	return updated
}

func findStart(garden []string) (int, int) {
	for i, row := range garden {
		for j, c := range row {
			if c == 'S' {
				return j, i
			}
		}
	}
	return -1, -1
}

// Return the number of garden plots reachable in steps steps.
func reach(garden []string, steps int, cycle bool) int {
	x, y := findStart(garden)

	plots := map[plot]bool{{x, y}: true}
	for i := 0; i < steps; i++ {
		plots = update(garden, plots, cycle)
	}
	return len(plots)
}

// Use quadratic extrapolation to compute the number of reachable plots using
// the simulated results from steps 65, 196 and 327.
func reachBig(garden []string, steps int) int {
	// steps = len(garden) * N + len(garden)/2
	n, m := len(garden), len(garden)/2
	if (steps-m)%n != 0 {
		panic("Invalid number of steps")
	}
	x, y := findStart(garden)

	var history []int
	var i int
	plots := map[plot]bool{{x, y}: true}
	for len(history) < 3 {
		plots = update(garden, plots, true)
		i++
		if (i-m)%n == 0 {
			history = append(history, len(plots))
		}
	}

	for i != steps {
		history = append(history[1:], day09.PredictNext(history))
		i += len(garden)
	}
	return history[2]
}
