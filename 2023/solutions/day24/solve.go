package day24

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

const day = 24

type hailstone struct {
	px, py, pz float64
	vx, vy, vz float64
}

const least = 200000000000000
const most = 400000000000000

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	hailstones := readInput(inputPath)
	fmt.Println(countIntersections(hailstones, least, most))
	fmt.Println(throwRock(hailstones))
}

func test(testInputPath string) {
	hailstones := readInput(testInputPath)
	solutions.AssertEq(countIntersections(hailstones, 7, 27), 2)
	solutions.AssertEq(throwRock(hailstones), 47)
}

func readInput(path string) []hailstone {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	hailstones := []hailstone{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		hailstones = append(hailstones, parseLine(scanner.Text()))
	}

	return hailstones
}

func parseLine(line string) hailstone {
	posVec := strings.Split(line, " @ ")

	var vec, pos []float64
	for _, p := range strings.Split(posVec[0], ",") {
		n, _ := strconv.Atoi(strings.TrimSpace(p))
		pos = append(pos, float64(n))
	}
	for _, v := range strings.Split(posVec[1], ",") {
		n, _ := strconv.Atoi(strings.TrimSpace(v))
		vec = append(vec, float64(n))
	}

	return hailstone{pos[0], pos[1], pos[2], vec[0], vec[1], vec[2]}
}

// Determine if two hailstones trajectories intersect in the future in some range.
func intersect(a, b hailstone, least, most float64) bool {
	// Parallel lines never intersect
	if a.vx*b.vy == a.vy*b.vx {
		return false
	}

	// Compute intersection of two parametric lines
	t1 := ((a.py-b.py)*b.vx + (b.px-a.px)*b.vy) / (a.vx*b.vy - a.vy*b.vx)
	t2 := (a.px - b.px + t1*a.vx) / b.vx

	minX := (least - a.px) / a.vx
	maxX := (most - a.px) / a.vx
	if a.vx < 0 {
		minX, maxX = maxX, minX
	}

	minY := (least - a.py) / a.vy
	maxY := (most - a.py) / a.vy
	if a.vy < 0 {
		minY, maxY = maxY, minY
	}

	return t1 > 0 && t2 > 0 && t1 >= max(minX, minY) && t1 <= min(maxX, maxY)
}

func countIntersections(hailstones []hailstone, least, most float64) int {
	count := 0
	for i := 0; i < len(hailstones); i++ {
		for j := i + 1; j < len(hailstones); j++ {
			if intersect(hailstones[i], hailstones[j], least, most) {
				count++
			}
		}
	}
	return count
}

// Compute the sum of rock starting position coordinates, which hits all hailstones.
func throwRock(hailstones []hailstone) int {
	// Use 3 hailstones from the input
	h := make([]hailstone, 3)
	copy(h, hailstones)

	// Convert positions relative to the first hailstone
	p1 := [3]float64{h[1].px - h[0].px, h[1].py - h[0].py, h[1].pz - h[0].pz}
	p2 := [3]float64{h[2].px - h[0].px, h[2].py - h[0].py, h[2].pz - h[0].pz}
	v1 := [3]float64{h[1].vx - h[0].vx, h[1].vy - h[0].vy, h[1].vz - h[0].vz}
	v2 := [3]float64{h[2].vx - h[0].vx, h[2].vy - h[0].vy, h[2].vz - h[0].vz}

	// Solve for times of collisions with (p1 + t1*v1) x (p2 + t2*v2) = 0
	t1 := mixedProduct(p2, p1, v2) / mixedProduct(v1, p2, v2)
	t2 := mixedProduct(p2, p1, v1) / mixedProduct(p1, v2, v1)

	// Compute collision points
	c1 := [3]float64{
		h[1].px + h[1].vx*t1,
		h[1].py + h[1].vy*t1,
		h[1].pz + h[1].vz*t1,
	}
	c2 := [3]float64{
		h[2].px + h[2].vx*t2,
		h[2].py + h[2].vy*t2,
		h[2].pz + h[2].vz*t2,
	}

	// Compute the velocity of the stone and back-calculate the position
	v := [3]float64{
		(c2[0] - c1[0]) / (t2 - t1),
		(c2[1] - c1[1]) / (t2 - t1),
		(c2[2] - c1[2]) / (t2 - t1),
	}
	return int(c1[0] - t1*v[0] + c1[1] - t1*v[1] + c1[2] - t1*v[2])
}

func mixedProduct(a, b, c [3]float64) float64 {
	return (a[1]*b[2]-a[2]*b[1])*c[0] +
		(a[2]*b[0]-a[0]*b[2])*c[1] +
		(a[0]*b[1]-a[1]*b[0])*c[2]
}
