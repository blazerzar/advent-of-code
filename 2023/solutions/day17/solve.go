package day17

import (
	"aoc-2023/solutions"
	"bufio"
	"container/heap"
	"fmt"
	"os"
)

const day = 17

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	city := readInput(inputPath)
	fmt.Println(minHeatLoss(city, 1, 3))
	fmt.Println(minHeatLoss(city, 4, 10))
}

func test(testInputPath string) {
	city := readInput(testInputPath)
	solutions.AssertEq(minHeatLoss(city, 1, 3), uint(102))
	solutions.AssertEq(minHeatLoss(city, 4, 10), uint(94))
}

func readInput(path string) [][]uint8 {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	var lines [][]uint8
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		numbers := make([]uint8, len(line))
		for i, c := range []byte(line) {
			numbers[i] = c - '0'
		}
		lines = append(lines, numbers)
	}

	return lines
}

const (
	up    = iota
	right = iota
	down  = iota
	left  = iota
)

var movesX = []int{0, 1, 0, -1}
var movesY = []int{-1, 0, 1, 0}

type position struct {
	x, y, direction int
}
type node struct {
	loc  position
	cost uint
}

// Compute the minimum heat loss by moving the crusible through the city. It
// must move in a straight line for at least minConsecutive steps and at most
// maxConsecutive steps.
func minHeatLoss(city [][]uint8, minConsecutive int, maxConsecutive int) uint {
	visited := make(map[position]struct{})
	pq := make(priorityQueue, 0)
	heap.Init(&pq)
	heap.Push(&pq, &node{loc: position{0, 0, right}, cost: 0})
	heap.Push(&pq, &node{loc: position{0, 0, down}, cost: 0})

	for pq.Len() > 0 {
		current := heap.Pop(&pq).(*node)
		loc := current.loc
		if loc.y == len(city)-1 && loc.x == len(city[loc.y])-1 {
			return current.cost
		}

		if _, ok := visited[loc]; ok {
			continue
		}
		visited[loc] = struct{}{}

		// Move in perpendicular directions for predefined number of steps
		dirs := []int{(loc.direction + 1) % 4, (loc.direction + 3) % 4}
		for _, d := range dirs {
			cost := current.cost
			for i := 1; i <= maxConsecutive; i++ {
				x, y := loc.x+movesX[d]*i, loc.y+movesY[d]*i
				if y < 0 || y >= len(city) || x < 0 || x >= len(city[y]) {
					break
				}
				cost += uint(city[y][x])
				if i >= minConsecutive {
					heap.Push(&pq, &node{loc: position{x, y, d}, cost: cost})
				}
			}
		}
	}

	return 0
}

type priorityQueue []*node

func (pq priorityQueue) Len() int           { return len(pq) }
func (pq priorityQueue) Less(i, j int) bool { return pq[i].cost < pq[j].cost }
func (pq priorityQueue) Swap(i, j int) {
	pq[i], pq[j] = pq[j], pq[i]
}

func (pq *priorityQueue) Push(x any) {
	*pq = append(*pq, x.(*node))
}

func (pq *priorityQueue) Pop() any {
	old := *pq
	n := len(old)
	x := old[n-1]
	*pq = old[0 : n-1]
	return x
}
