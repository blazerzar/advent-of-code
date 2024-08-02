package day14

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"reflect"
)

const day = 14

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	rocks := readInput(inputPath)
	fmt.Println(northWallLoad(rocks))
	fmt.Println(loadAfterCycles(rocks, 1e9))
}

func test(testInputPath string) {
	rocks := readInput(testInputPath)
	solutions.AssertEq(northWallLoad(rocks), 136)
	solutions.AssertEq(loadAfterCycles(rocks, 1e9), 64)
}

func readInput(path string) [][]byte {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	lines := [][]byte{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		lines = append(lines, []byte(scanner.Text()))
	}

	return lines
}

func wallLoad(rocks [][]byte) int {
	var load int
	for i, row := range rocks {
		for _, c := range row {
			if c == 'O' {
				load += len(rocks) - i
			}
		}
	}
	return load
}

// Compute load after tilting the plane north.
func northWallLoad(r [][]byte) int {
	rocks := make([][]byte, len(r))
	for i, row := range r {
		rocks[i] = make([]byte, len(row))
		copy(rocks[i], row)
	}

	tiltRocks(rocks, 0, -1)
	return wallLoad(rocks)
}

// Simulate the tilting of the plane. Variables dx and dy determine the
// direction of the tilt. During the simulation, we move in the opposite
// direction to move the "lower" rocks first.
func tiltRocks(rocks [][]byte, dx int, dy int) {
	outerLimit := len(rocks)
	innerLimit := len(rocks[0])
	// For tilting north and south, we update each column instead of each row
	if dy != 0 {
		outerLimit, innerLimit = innerLimit, outerLimit
	}

	for i := 0; i < outerLimit; i++ {
		// Index to place the next rock
		var rockIndex int
		for j := 0; j < innerLimit; j++ {
			x, y, rockX, rockY := j, i, rockIndex, i
			// For south and east tilts, the inner loop is reversed
			if dy == 1 || dx == 1 {
				x, rockX = innerLimit-j-1, innerLimit-rockIndex-1
			}
			// For north and south tilts, we transpose the coordinates
			if dy != 0 {
				x, y, rockX, rockY = y, x, rockY, rockX
			}

			switch rocks[y][x] {
			case 'O':
				rocks[y][x] = '.'
				rocks[rockY][rockX] = 'O'
				rockIndex++
			case '#':
				rockIndex = j + 1
			}
		}
	}
}

func cycleRocks(rocks [][]byte) {
	tiltRocks(rocks, 0, -1)
	tiltRocks(rocks, -1, 0)
	tiltRocks(rocks, 0, 1)
	tiltRocks(rocks, 1, 0)
}

// Compute the load on the north wall after a given number of cycles. We
// simulate the cycles until a repeating pattern is found and the load can
// be computed directly.
func loadAfterCycles(r [][]byte, cycles int) int {
	rocks := make([][]byte, len(r))
	for i, row := range r {
		rocks[i] = make([]byte, len(row))
		copy(rocks[i], row)
	}

	loads := []int{wallLoad(rocks)}
	for i := 0; i < cycles; i++ {
		cycleRocks(rocks)
		loads = append(loads, wallLoad(rocks))

		// Check every possible period length
		for period := 2; 2*period < len(loads); period++ {
			prev, curr := len(loads)-2*period, len(loads)-period
			if !reflect.DeepEqual(loads[prev:curr], loads[curr:]) {
				continue
			}

			// Remove loads before the start of the pattern and find location
			// inside the pattern
			return loads[prev+(cycles-prev)%period]
		}

	}

	// No repeating pattern found
	return loads[len(loads)-1]
}
