package day10

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strings"
)

const day = 10

type pair struct {
	x, y int
}

type neighbour struct {
	dx, dy                      int
	validPipesIn, validPipesOut string
}

var neighbours = []neighbour{
	{0, 1, "S|7F", "|LJ"}, {0, -1, "S|LJ", "|7F"},
	{1, 0, "S-LF", "-J7"}, {-1, 0, "S-J7", "-LF"}}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath1 := fmt.Sprintf("inputs/day_%02d_test_1.txt", day)
	testInputPath2 := fmt.Sprintf("inputs/day_%02d_test_2.txt", day)
	test(testInputPath1, testInputPath2)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	pipes := readInput(inputPath)
	fmt.Println(farthestPoint(pipes))
	fmt.Println(enclosedTiles(pipes))
}

func test(testInputPath1 string, testInputPath2 string) {
	testPipes := readInput(testInputPath1)
	solutions.AssertEq(farthestPoint(testPipes), 8)
	testPipes = readInput(testInputPath2)
	solutions.AssertEq(enclosedTiles(testPipes), 8)
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

// Find distance to the farthest point from the starting point S.
func farthestPoint(pipes []string) int {
	x, y := startingPoint(pipes)
	loop := findLoop(pipes, x, y)
	return len(loop) / 2
}

// Count number of tiles enclosed by the main loop of pipes.
func enclosedTiles(pipes []string) int {
	x, y := startingPoint(pipes)
	loop := findLoop(pipes, x, y)

	// Pick's theorem: area = inside + boundary / 2 - 1
	area := 0
	for i := 0; i < len(loop); i++ {
		j := (i + 1) % len(loop)
		// Shoelace formula
		area += (loop[i].x - loop[j].x) * (loop[j].y + loop[i].y)
	}
	if area < 0 {
		area = -area
	}
	boundary := len(loop) - 1

	// Divide at the end, because we are dealing with integers
	return (area - boundary + 2) / 2
}

func startingPoint(pipes []string) (int, int) {
	m := len(pipes)
	n := len(pipes[0])

	for i := 0; i < m*n; i++ {
		if pipes[i/n][i%n] == 'S' {
			return i % n, i / n
		}
	}

	return -1, -1
}

// Recursively find the main loop of pipes starting from (x, y) usin DFS.
func findLoop(pipes []string, x int, y int) []pair {
	loop := []pair{{x, y}}

	// Mark pipe visited
	c := pipes[y][x]
	marked := []rune(pipes[y])
	marked[x] = '.'
	pipes[y] = string(marked)

	// Check neighbours for possible next pipe
	for _, ns := range neighbours {
		// Check valid previous pipe for this direction
		if !strings.Contains(ns.validPipesIn, string(c)) {
			continue
		}

		nx := x + ns.dx
		ny := y + ns.dy
		if ny < 0 || ny >= len(pipes) || nx < 0 || nx >= len(pipes[ny]) {
			continue
		}

		// Check valid next pipe for this direction
		if !strings.Contains(ns.validPipesOut, string(pipes[ny][nx])) {
			continue
		}

		loop = append(loop, findLoop(pipes, nx, ny)...)
		break
	}

	// Mark pipe unvisited
	marked[x] = rune(c)
	pipes[y] = string(marked)

	return loop
}
