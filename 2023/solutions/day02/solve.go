package day02

import (
	"aoc-2023/solutions"
	"fmt"
	"os"
	"strconv"
	"strings"
)

const day = 2

type cubes struct {
	red   int
	green int
	blue  int
}

type game struct {
	id    int
	cubes []cubes
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	games := readInput(inputPath)
	fmt.Println(part1(games))
	fmt.Println(part2(games))
}

func test(testInputPath string) {
	testGames := readInput(testInputPath)
	solutions.AssertEq(part1(testGames), 8)
	solutions.AssertEq(part2(testGames), 2286)
}

func readInput(path string) []game {
	dat, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	games := []game{}
	for _, line := range strings.Split(string(dat), "\n") {
		if len(line) == 0 {
			continue
		}

		// Parsing game id
		split := strings.Split(line, ": ")
		gameID, _ := strconv.Atoi(split[0][5:])

		// Parsing number of cubes
		split = strings.Split(split[1], "; ")
		cubes := make([]cubes, len(split))
		for i, reach := range split {
			split = strings.Split(reach, ", ")

			for _, cube := range split {
				// Compare strings to figure out cube color, length check not
				// needed since cube string length is at least 5
				if cube[len(cube)-3:] == "red" {
					cubes[i].red, _ = strconv.Atoi(cube[:len(cube)-4])
				} else if cube[len(cube)-5:] == "green" {
					cubes[i].green, _ = strconv.Atoi(cube[:len(cube)-6])
				} else if cube[len(cube)-4:] == "blue" {
					cubes[i].blue, _ = strconv.Atoi(cube[:len(cube)-5])
				}
			}
		}

		games = append(games, game{id: gameID, cubes: cubes})
	}

	return games
}

// Sum ids of possible games
func part1(games []game) int {
	availableCubes := cubes{red: 12, green: 13, blue: 14}
	idSum := 0

	for _, game := range games {
		possible := true

		for _, cubes := range game.cubes {
			if cubes.red > availableCubes.red ||
				cubes.green > availableCubes.green ||
				cubes.blue > availableCubes.blue {
				possible = false
				break
			}
		}

		if possible {
			idSum += game.id
		}
	}

	return idSum
}

// Find minimum number of cubes for games to be possible and sum their products
func part2(games []game) int {
	powerSum := 0

	for _, game := range games {
		maxRed, maxGreen, maxBlue := 0, 0, 0
		for _, cubes := range game.cubes {
			maxRed = max(maxRed, cubes.red)
			maxGreen = max(maxGreen, cubes.green)
			maxBlue = max(maxBlue, cubes.blue)
		}
		powerSum += maxRed * maxGreen * maxBlue
	}

	return powerSum
}
