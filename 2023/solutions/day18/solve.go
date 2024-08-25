package day18

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
)

const day = 18

var moves = map[byte][]int{
	'U': {0, -1},
	'R': {1, 0},
	'D': {0, 1},
	'L': {-1, 0},
}

type instruction struct {
	direction byte
	length    uint
	color     uint
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	instructions := readInput(inputPath)
	fmt.Println(lagoonSize(instructions))
	fmt.Println(lagoonSize(extractInstructions(instructions)))
}

func test(testInputPath string) {
	instructions := readInput(testInputPath)
	solutions.AssertEq(lagoonSize(instructions), 62)
	solutions.AssertEq(lagoonSize(extractInstructions(instructions)), 952408144115)
}

func readInput(path string) []instruction {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	instructions := []instruction{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		instruct := instruction{}
		fmt.Sscanf(scanner.Text(), "%c %d (#%x)",
			&instruct.direction, &instruct.length, &instruct.color)
		instructions = append(instructions, instruct)
	}

	return instructions
}

// Use the shoelace formula to compute the area used in the Pick's theorem.
func lagoonSize(instructions []instruction) int {
	var x, y, doubleArea, boundary int
	for i := 0; i < len(instructions); i++ {
		curr := instructions[i]
		length := curr.length
		newX := x + moves[curr.direction][0]*int(length)
		newY := y + moves[curr.direction][1]*int(length)

		doubleArea += (x - newX) * (y + newY)
		boundary += int(length)
		x, y = newX, newY
	}
	if doubleArea < 0 {
		doubleArea = -doubleArea
	}

	// Pick's theorem: A = i + b/2 - 1 => i = (2*A - b + 2) / 2
	interior := (doubleArea - boundary + 2) / 2
	return interior + boundary
}

var colors = [4]byte{'R', 'D', 'L', 'U'}

// Extract corrent instructions from the color.
func extractInstructions(instructions []instruction) []instruction {
	var extracted []instruction
	for _, instruct := range instructions {
		extracted = append(extracted, instruction{
			direction: colors[instruct.color&3],
			length:    instruct.color / 16,
		})
	}
	return extracted
}
