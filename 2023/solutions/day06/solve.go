package day06

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
)

const day = 6

type race struct {
	time     int
	distance int
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	races := readInput(inputPath)
	fmt.Println(part1(races))
	fmt.Println(part2(races))
}

func test(testInputPath string) {
	races := readInput(testInputPath)
	solutions.AssertEq(part1(races), 288)
	solutions.AssertEq(part2(races), 71503)
}

func part1(races []race) int {
	result := 1

	for _, r := range races {
		result *= waysToRecord(r)
	}

	return result
}

func part2(races []race) int {
	// Create one race from all races
	time := ""
	distance := ""

	for _, r := range races {
		time += strconv.Itoa(r.time)
		distance += strconv.Itoa(r.distance)
	}

	t, _ := strconv.Atoi(time)
	d, _ := strconv.Atoi(distance)
	r := race{time: t, distance: d}

	return waysToRecord(r)
}

// Return number of ways to win in a race
func waysToRecord(r race) int {
	// Solve -h^2 + ht - d > 0
	D := float64(r.time*r.time - 4*r.distance)
	low := math.Floor((-float64(r.time)+math.Sqrt(D))/-2) + 1
	high := math.Ceil((-float64(r.time)-math.Sqrt(D))/-2) - 1
	return int(high - low + 1)
}

func readInput(path string) []race {
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

	times := strings.Fields(lines[0])
	distances := strings.Fields(lines[1])
	races := []race{}

	for i := 1; i < len(times); i++ {
		time, _ := strconv.Atoi(times[i])
		distance, _ := strconv.Atoi(distances[i])
		races = append(races, race{time: time, distance: distance})
	}

	return races
}
