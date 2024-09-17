package day22

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"slices"
	"strconv"
	"strings"
)

const day = 22

type coordinate struct {
	x, y int
}
type brick struct {
	blocks     []coordinate
	z          int
	height     int
	supporting map[int]bool
	supported  map[int]bool
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	bricks := readInput(inputPath)
	simulate(bricks)
	fmt.Println(safeToDisintegrate(bricks))
	fmt.Println(fallingBricksSum(bricks))
}

func test(testInputPath string) {
	bricks := readInput(testInputPath)
	simulate(bricks)
	solutions.AssertEq(safeToDisintegrate(bricks), 5)
	solutions.AssertEq(fallingBricksSum(bricks), 7)
}

func readInput(path string) []brick {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	bricks := []brick{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		b := createBrick(parseLine(scanner.Text()))
		bricks = append(bricks, b)
	}
	slices.SortFunc(bricks, func(a, b brick) int {
		return a.z - b.z
	})

	return bricks
}

// Create a brick from start and end coordinates. The blocks represent the
// coordinates of the brick on the x-y plane.
func createBrick(start, end []int) brick {
	b := brick{
		z:          start[2],
		height:     end[2] - start[2] + 1,
		supporting: make(map[int]bool),
		supported:  make(map[int]bool),
	}

	dx, dy, dz := end[0]-start[0], end[1]-start[1], end[2]-start[2]
	dx, dy, dz = dx/max(dx, -dx, 1), dy/max(dy, -dy, 1), dz/max(dz, -dz, 1)

	x, y, z := start[0], start[1], start[2]
	for {
		b.blocks = append(b.blocks, coordinate{x, y})
		if x == end[0] && y == end[1] && z == end[2] || dz != 0 {
			break
		}
		x += dx
		y += dy
		z += dz
	}

	return b
}

// Parse line into brick start and end points.
func parseLine(line string) ([]int, []int) {
	ends := strings.Split(line, "~")
	var start, end []int

	s := strings.Split(ends[0], ",")
	e := strings.Split(ends[1], ",")
	for i := range s {
		n, _ := strconv.Atoi(s[i])
		start = append(start, n)
		n, _ = strconv.Atoi(e[i])
		end = append(end, n)
	}

	return start, end
}

// Initialize an empty 3D grid with 0 values representing empty cells.
func initGrid(bricks []brick) [][][]int {
	maxX, maxY, maxZ := 0, 0, 0
	for _, brick := range bricks {
		n := len(brick.blocks)
		maxX = max(maxX, brick.blocks[n-1].x)
		maxY = max(maxY, brick.blocks[n-1].y)
		maxZ = max(maxZ, brick.z+brick.height-1)
	}

	grid := make([][][]int, maxX+1)
	for x := 0; x <= maxX; x++ {
		grid[x] = make([][]int, maxY+1)
		for y := 0; y <= maxY; y++ {
			grid[x][y] = make([]int, maxZ+1)
		}
	}

	return grid
}

// Simulate the bricks falling and landing on the ground.
func simulate(bricks []brick) [][][]int {
	grid := initGrid(bricks)

	for i, brick := range bricks {
		// Bricks fall until they reach the ground or another brick
		for {
			fall := true
			for _, block := range brick.blocks {
				below := grid[block.x][block.y][brick.z-1]
				if below != 0 {
					fall = false
					bricks[i].supported[below-1] = true
					bricks[below-1].supporting[i] = true
				}
			}
			if !fall || brick.z == 1 {
				break
			}
			brick.z--
		}

		// Place the brick into the grid
		for _, block := range brick.blocks {
			for h := 0; h < brick.height; h++ {
				grid[block.x][block.y][brick.z+h] = i + 1
			}
		}
	}

	return grid
}

// Return the number of bricks that can be safely disintegrated - removing one
// of them will not cause any other brick to fall.
func safeToDisintegrate(bricks []brick) int {
	var count int
	for i := range bricks {
		if fallingBricks(bricks, i) == 0 {
			count++
		}
	}
	return count
}

// Return the number of bricks that will fall if the i-th brick is disintegrated.
func fallingBricks(bricks []brick, i int) int {
	bricks = copyBricks(bricks)
	var count int
	q := []int{i}

	for len(q) > 0 {
		b := q[0]
		q = q[1:]
		for supporting := range bricks[b].supporting {
			if len(bricks[supporting].supported) == 1 {
				q = append(q, supporting)
				count++
			}
			delete(bricks[supporting].supported, b)
		}
	}
	return count
}

func fallingBricksSum(bricks []brick) int {
	var sum int
	for i := range bricks {
		sum += fallingBricks(bricks, i)
	}
	return sum
}

func copyBricks(bricks []brick) []brick {
	copied := make([]brick, len(bricks))
	for i, b := range bricks {
		copied[i] = b
		copied[i].supporting = make(map[int]bool, len(b.supporting))
		for k, v := range b.supporting {
			copied[i].supporting[k] = v
		}
		copied[i].supported = make(map[int]bool, len(b.supported))
		for k, v := range b.supported {
			copied[i].supported[k] = v
		}
	}
	return copied
}
