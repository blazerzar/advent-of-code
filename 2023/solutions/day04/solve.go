package day04

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
)

const day = 4

type card struct {
	winning [100]bool
	mine    [100]bool
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	cards := readInput(inputPath)
	fmt.Println(countPoints(cards))
	fmt.Println(countScratchcards(cards))
}

func test(testInputPath string) {
	testInput := readInput(testInputPath)
	solutions.AssertEq(countPoints(testInput), 13)
	solutions.AssertEq(countScratchcards(testInput), 30)
}

func readInput(path string) []card {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	cards := make([]card, 0)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := string(scanner.Text())
		semi := strings.Index(line, ":")
		pipe := strings.Index(line, "|")

		c := card{}
		// Use the fact that every number takes 2 characters
		for i := semi + 2; i < pipe; i += 3 {
			num, _ := strconv.Atoi(strings.TrimSpace(line[i : i+2]))
			c.winning[num] = true
		}
		for i := pipe + 2; i < len(line); i += 3 {
			num, _ := strconv.Atoi(strings.TrimSpace(line[i : i+2]))
			c.mine[num] = true
		}
		cards = append(cards, c)
	}

	return cards
}

// Count number of matches between winning and mine numbers.
func cardMatches(c card) int {
	matches := 0
	for i := 0; i < 100; i++ {
		if c.winning[i] && c.mine[i] {
			matches++
		}
	}
	return matches
}

// Count total number of points for all cards.
func countPoints(cards []card) int {
	points := 0

	for _, c := range cards {
		matches := cardMatches(c)
		if matches > 0 {
			points += int(math.Pow(2, float64(matches-1)))
		}
	}

	return points
}

// Count number of scratchcards that are won.
func countScratchcards(cards []card) int {
	scratchcards := make([]int, len(cards))
	// We start with 1 copy of each
	for i := 0; i < len(scratchcards); i++ {
		scratchcards[i] = 1
	}

	for scratchcard, count := range scratchcards {
		matches := cardMatches(cards[scratchcard])
		for i := 0; i < matches; i++ {
			// Each scratchcard wins one copy of the next [matches] scratchcards
			scratchcards[scratchcard+1+i] += count
		}
	}

	sum := 0
	for _, count := range scratchcards {
		sum += count
	}

	return sum
}
