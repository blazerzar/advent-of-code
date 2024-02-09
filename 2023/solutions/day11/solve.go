package day11

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
)

const day = 11

type galaxy struct {
	x, y int
}

type spaceImage struct {
	galaxies []galaxy
	// Cumulative number of empty columns and rows for each index
	emptyColumns []int
	emptyRows    []int
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	space := readInput(inputPath)
	fmt.Println(shortestPaths(space, 2))
	fmt.Println(shortestPaths(space, 1_000_000))
}

func test(testInputPath string) {
	space := readInput(testInputPath)
	solutions.AssertEq(shortestPaths(space, 2), 374)
	solutions.AssertEq(shortestPaths(space, 10), 1030)
	solutions.AssertEq(shortestPaths(space, 100), 8410)
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

func parseInput(image []string) spaceImage {
	spImage := spaceImage{
		galaxies:     []galaxy{},
		emptyColumns: make([]int, len(image[0])),
		emptyRows:    make([]int, len(image)),
	}

	galaxiesColumns := make([]int, len(image[0]))
	galaxiesRows := make([]int, len(image))

	for y, row := range image {
		for x, cell := range row {
			if cell == '#' {
				spImage.galaxies = append(spImage.galaxies, galaxy{x, y})
				galaxiesColumns[x]++
				galaxiesRows[y]++
			}
		}
	}

	// Calculate empty columns and rows
	for i, count := range galaxiesColumns {
		if i > 0 {
			spImage.emptyColumns[i] = spImage.emptyColumns[i-1]
		}
		if count == 0 {
			spImage.emptyColumns[i]++
		}
	}

	for i, count := range galaxiesRows {
		if i > 0 {
			spImage.emptyRows[i] = spImage.emptyRows[i-1]
		}
		if count == 0 {
			spImage.emptyRows[i]++
		}
	}

	return spImage
}

// Find sum of shortest paths between all pairs of galaxies.
func shortestPaths(image []string, empty int) int {
	spImage := parseInput(image)
	sum := 0

	for i := 0; i < len(spImage.galaxies); i++ {
		for j := i + 1; j < len(spImage.galaxies); j++ {
			sum += shortestPath(i, j, spImage, empty)
		}
	}

	return sum
}

// Compute shortest path between two galaxies, where each empty column
// (and row) expands to empty number of columns (and rows).
func shortestPath(i, j int, image spaceImage, empty int) int {
	// Compute Manhattan distance between galaxies (diagonal moves not allowed)
	distX := image.galaxies[i].x - image.galaxies[j].x
	distX = max(-distX, distX)
	distY := image.galaxies[i].y - image.galaxies[j].y
	distY = max(-distY, distY)

	x1 := min(image.galaxies[i].x, image.galaxies[j].x)
	x2 := max(image.galaxies[i].x, image.galaxies[j].x)
	y1 := min(image.galaxies[i].y, image.galaxies[j].y)
	y2 := max(image.galaxies[i].y, image.galaxies[j].y)

	// Compute number of empty columns and rows between galaxies, which
	// expand the galaxy and increase the shortest path
	expandX := (image.emptyColumns[x2] - image.emptyColumns[x1]) * (empty - 1)
	expandY := (image.emptyRows[y2] - image.emptyRows[y1]) * (empty - 1)

	return distX + distY + expandX + expandY
}
