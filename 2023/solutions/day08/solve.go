package day08

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"regexp"
)

const day = 8

type maps struct {
	instructions []int
	network      map[string][2]string
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath1 := fmt.Sprintf("inputs/day_%02d_test_1.txt", day)
	testInputPath2 := fmt.Sprintf("inputs/day_%02d_test_2.txt", day)
	test(testInputPath1, testInputPath2)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	m := readInput(inputPath)
	fmt.Println(pathLength(m))
	fmt.Println(allPathsLength(m))
}

func test(testInputPath1 string, testInputPath2 string) {
	testMaps := readInput(testInputPath1)
	solutions.AssertEq(pathLength(testMaps), 2)
	testMaps = readInput(testInputPath2)
	solutions.AssertEq(allPathsLength(testMaps), 6)
}

func readInput(path string) maps {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	maps := maps{}
	scanner := bufio.NewScanner(file)

	// Read instructions
	scanner.Scan()
	for _, c := range scanner.Text() {
		var i int
		if c == 'L' {
			i = 0
		} else {
			i = 1
		}
		maps.instructions = append(maps.instructions, i)
	}

	maps.network = make(map[string][2]string)
	re := regexp.MustCompile(`(\w{3}) = \((\w{3}), (\w{3})\)`)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			continue
		}

		matches := re.FindStringSubmatch(line)
		maps.network[matches[1]] = [2]string{matches[2], matches[3]}
	}

	return maps
}

// Return length of path from "AAA" to "ZZZ"
func pathLength(m maps) int {
	length := 0
	current := "AAA"

	for current != "ZZZ" {
		current = m.network[current][m.instructions[length%len(m.instructions)]]
		length++
	}

	return length
}

// Return length of path from all nodes ending with an "A" to all nodes
// ending with "Z"
func allPathsLength(m maps) int {
	// Get period for each starting node (phase shift equals period for all nodes)
	periods := []int{}

	for node := range m.network {
		if node[2] != 'A' {
			continue
		}

		length := 0
		for node[2] != 'Z' {
			node = m.network[node][m.instructions[length%len(m.instructions)]]
			length++
		}
		periods = append(periods, length)
	}

	return lcm(periods)
}

func gcd(a, b int) int {
	for b != 0 {
		t := b
		b = a % b
		a = t
	}
	return a
}

func lcm(nums []int) int {
	result := nums[0]
	for i := 1; i < len(nums); i++ {
		result = result * nums[i] / gcd(result, nums[i])
	}
	return result
}
