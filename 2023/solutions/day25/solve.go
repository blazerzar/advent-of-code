package day25

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"math/rand"
	"os"
	"strings"
)

const day = 25

type graph struct {
	vertices map[string]bool
	edges    [][2]string
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	g := readInput(inputPath)
	fmt.Println(cut(g))
}

func test(testInputPath string) {
	g := readInput(testInputPath)
	solutions.AssertEq(cut(g), 54)
}

func readInput(path string) graph {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	g := graph{vertices: make(map[string]bool)}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		node, edges := parseLine(scanner.Text())
		g.vertices[node] = true
		for _, edge := range edges {
			g.vertices[edge] = true
			g.edges = append(g.edges, [2]string{node, edge})
		}
	}

	return g
}

func parseLine(line string) (string, []string) {
	nodeEdges := strings.Split(line, ": ")
	return nodeEdges[0], strings.Split(nodeEdges[1], " ")
}

// Return a new graph with one edge contracted.
func contract(g graph) graph {
	e := g.edges[rand.Intn(len(g.edges))]
	v := e[0] + e[1]

	// Create a new graph with two vertices contracted into one
	contracted := graph{vertices: map[string]bool{v: true}}
	for node := range g.vertices {
		if node != e[0] && node != e[1] {
			contracted.vertices[node] = true
		}
	}

	// Replace removed vertices with the new one in the edges
	for _, edge := range g.edges {
		if edge == e || edge[0] == e[1] && edge[1] == e[0] {
			continue
		}

		if edge[0] == e[0] || edge[0] == e[1] {
			edge[0] = v
		} else if edge[1] == e[0] || edge[1] == e[1] {
			edge[1] = v
		}
		contracted.edges = append(contracted.edges, edge)

	}

	return contracted
}

func karger(g graph) graph {
	for len(g.vertices) > 2 {
		g = contract(g)
	}
	return g
}

// Run Karger's algorithm until a minimum cut of size 3 is found.
func cut(g graph) int {
	cg := karger(g)
	for len(cg.edges) != 3 {
		cg = karger(g)
	}
	return len(cg.edges[0][0]) * len(cg.edges[0][1]) / 9
}
