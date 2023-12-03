package day03

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strconv"
)

const day = 3

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	input := readInput(inputPath)
	fmt.Println(part1(input))
	fmt.Println(part2(input))
}

func test(testInputPath string) {
	schematic := readInput(testInputPath)
	solutions.AssertEq(part1(schematic), 4361)
	solutions.AssertEq(part2(schematic), 467835)
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

// Returns sum of part numbers.
func part1(lines []string) int {
	partNumbersSum := 0
	digitStart := -1

	for i, line := range lines {
		for j, c := range line {
			if c >= '0' && c <= '9' && digitStart == -1 {
				digitStart = j
			} else if (c < '0' || c > '9' || j == len(line)-1) && digitStart != -1 {
				// Increment if current character is a digit
				if c >= '0' && c <= '9' {
					j++
				}

				if isPartNumber(lines, i, digitStart, j) {
					num, _ := strconv.Atoi(line[digitStart:j])
					partNumbersSum += num
				}
				digitStart = -1
			}
		}
	}

	return partNumbersSum
}

// Returns true if number at line i and columns [digitStart:j] is adjecent to
// a symbol.
func isPartNumber(lines []string, i int, digitStart int, j int) bool {
	for y := i - 1; y <= i+1; y++ {
		if y < 0 || y >= len(lines) {
			continue
		}

		for x := digitStart - 1; x <= j; x++ {
			if x < 0 || x >= len(lines[y]) {
				continue
			}

			c := lines[y][x]
			if c != '.' && (c < '0' || c > '9') {
				return true
			}
		}
	}

	return false
}

type gear struct {
	x, y  int
	ratio int
	valid bool
}

// Returns sum of gear ratios.
func part2(lines []string) int {
	gears := []gear{}
	digitStart := -1

	for i, line := range lines {
		for j, c := range line {
			if c >= '0' && c <= '9' && digitStart == -1 {
				digitStart = j
			} else if (c < '0' || c > '9' || j == len(line)-1) && digitStart != -1 {
				// Increment if current character is a digit
				if c >= '0' && c <= '9' {
					j++
				}

				x, y := adjecentToGear(lines, i, digitStart, j)
				if x != -1 && y != -1 {
					num, _ := strconv.Atoi(line[digitStart:j])

					// Check if gear already exists
					for idx, gear := range gears {
						if gear.x == x && gear.y == y {
							gears[idx].ratio *= num
							gears[idx].valid = true
							num = -num
							break
						}
					}

					// New gear is created (num was negated)
					if num > 0 {
						gears = append(gears, gear{x, y, num, false})
					}
				}
				digitStart = -1
			}
		}
	}

	sum := 0
	for _, gear := range gears {
		if gear.valid {
			sum += gear.ratio
		}
	}

	return sum
}

// Returns valid gear coordinates if number is adjacent to it
func adjecentToGear(lines []string, i int, digitStart int, j int) (int, int) {
	for y := i - 1; y <= i+1; y++ {
		if y < 0 || y >= len(lines) {
			continue
		}

		for x := digitStart - 1; x <= j; x++ {
			if x < 0 || x >= len(lines[y]) {
				continue
			}

			if lines[y][x] == '*' {
				return x, y
			}
		}
	}

	return -1, -1
}
