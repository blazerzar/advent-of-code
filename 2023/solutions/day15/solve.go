package day15

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strings"
)

const day = 15

type lens struct {
	label string
	focal int
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	instructions := readInput(inputPath)
	fmt.Println(verificationNumber(instructions))
	fmt.Println(hashmap(instructions))
}

func test(testInputPath string) {
	solutions.AssertEq(hash("HASH"), 52)
	instructions := readInput(testInputPath)
	solutions.AssertEq(verificationNumber(instructions), 1320)
	solutions.AssertEq(hashmap(instructions), 145)
}

func readInput(path string) []string {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	scanner.Scan()
	instructions := scanner.Text()
	return strings.Split(instructions, ",")
}

func hash(s string) uint8 {
	var value uint8
	for _, c := range []byte(s) {
		// Computed module 256 because of uint8
		value = 17 * (value + c)

	}
	return value
}

func verificationNumber(instructions []string) uint {
	var verification uint
	for _, instruction := range instructions {
		verification += uint(hash(instruction))
	}
	return verification
}

// Holiday ASCII String Helper Manual Arrangement Procedure
func hashmap(instructions []string) int {
	var boxes [256][]lens

	for _, instruction := range instructions {
		var label string
		if index := strings.Index(instruction, "="); index != -1 {
			label = instruction[:index]
		} else {
			label = instruction[:len(instruction)-1]
		}
		box := hash(label)

		slot := -1
		for i, l := range boxes[box] {
			if l.label == label {
				slot = i
				break
			}
		}

		// Instructions with = are of the form "label=focal"
		if len(instruction)-2 == len(label) {
			focal := int(byte(instruction[len(instruction)-1]) - '0')
			// Add lens / set focal length
			if slot != -1 {
				boxes[box][slot].focal = focal
			} else {
				boxes[box] = append(boxes[box], lens{label, focal})
			}
		} else if slot != -1 {
			// Remove lens
			boxes[box] = append(boxes[box][:slot], boxes[box][slot+1:]...)
		}
	}

	var focusingPower int
	for i, box := range boxes {
		for slot, lens := range box {
			focusingPower += (i + 1) * (slot + 1) * lens.focal
		}
	}
	return focusingPower
}
