package day19

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"strings"
)

const day = 19

type rule struct {
	category    int
	direction   int
	threshold   int
	destination string
}
type workflow []rule
type rating [4]int

var categories = map[byte]int{'x': 0, 'm': 1, 'a': 2, 's': 3}
var directions = map[byte]int{'<': 1, '>': -1}

const (
	minRating = 1
	maxRating = 4000
)

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	workflows, parts := readInput(inputPath)
	fmt.Println(acceptedParts(workflows, parts))
	fmt.Println(distinctCombinations(workflows))
}

func test(testInputPath string) {
	workflows, parts := readInput(testInputPath)
	solutions.AssertEq(acceptedParts(workflows, parts), 19114)
	solutions.AssertEq(distinctCombinations(workflows), 167409079868000)
}

func readInput(path string) (map[string]workflow, []rating) {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)

	workflows := make(map[string]workflow)
	for scanner.Scan() {
		line := scanner.Text()
		if line == "" {
			break
		}
		name, w := parseWorkflow(line)
		workflows[name] = w
	}
	var parts []rating
	for scanner.Scan() {
		parts = append(parts, parseRating(scanner.Text()))
	}

	return workflows, parts
}

func parseWorkflow(line string) (string, workflow) {
	brace := strings.Index(line, "{")
	name := line[:brace]
	rules := line[brace+1 : len(line)-1]

	var w workflow
	for _, r := range strings.Split(rules, ",") {
		split := strings.Split(r, ":")
		ru := rule{destination: split[len(split)-1]}
		if len(split) == 2 {
			ru.category = categories[split[0][0]]
			ru.direction = directions[split[0][1]]
			fmt.Sscanf(split[0], split[0][:2]+"%d", &ru.threshold)
		} else {
			ru.direction = directions['>']
		}

		w = append(w, ru)
	}
	return name, w
}

func parseRating(line string) rating {
	var r rating
	fmt.Sscanf(line, "{x=%d,m=%d,a=%d,s=%d}", &r[0], &r[1], &r[2], &r[3])
	return r
}

// Return the next workflow after sending the part through the current one.
func nextWorkflow(w workflow, part rating) string {
	for _, r := range w {
		if part[r.category]*r.direction < r.threshold*r.direction {
			return r.destination
		}
	}
	panic("No rule matched")
}

// Return the rating sum if the part is accepted, 0 otherwise.
func accept(workflows map[string]workflow, part rating) int {
	current := "in"

	for {
		current = nextWorkflow(workflows[current], part)
		if current == "A" {
			return part[0] + part[1] + part[2] + part[3]
		} else if current == "R" {
			return 0
		}
	}
}

// Solve part 1 by summing the ratings of the accepted parts.
func acceptedParts(workflows map[string]workflow, parts []rating) int {
	var sum int
	for _, p := range parts {
		sum += accept(workflows, p)
	}
	return sum
}

type workflowRule struct {
	workflow string
	rule     int
}

// Invert the workflows to map destinations to their source and find the rules
// which lead to an accepted part.
func invertWorkflows(workflows map[string]workflow) (map[string]workflowRule, []workflowRule) {
	reverseWorkflows := make(map[string]workflowRule)
	accepts := []workflowRule{}

	for name, w := range workflows {
		for i, r := range w {
			if _, ok := reverseWorkflows[r.destination]; ok {
				panic("Multiple rules to the same destination")
			}
			wr := workflowRule{name, i}
			if r.destination == "A" {
				accepts = append(accepts, wr)
			} else if r.destination != "R" {
				reverseWorkflows[r.destination] = wr
			}
		}
	}

	return reverseWorkflows, accepts
}

// Update the combinations to be accepted or rejected based on the rule r. At
// the beginning all combinations are valid (0) and we mark the invalid (1) ones
// in each iteration. If a rule is accepted, the ratings [T, maxRating] for <,
// and [minRating, T] for > are invalid. If a rule is rejected, the ratings
// [minRating, T-1] for <, and [T+1, maxRating] for > are invalid.
func updateCombinations(combinations [][]int, r rule, accept bool) {
	rating := r.threshold
	update := r.direction
	if !accept {
		rating -= r.direction
		update *= -1
	}

	for ; rating >= minRating && rating <= maxRating; rating += update {
		combinations[r.category][rating] = 1
	}
}

func countCombinations(combinations [][]int) int {
	count := 1
	for _, c := range combinations {
		sum := 0
		for i := minRating; i <= maxRating; i++ {
			// Accepted combinations are marked with 0
			sum += 1 - c[i]
		}
		count *= sum
	}
	return count
}

// Solve part 2 by counting the number of distinct accepted combinations.
func distinctCombinations(workflows map[string]workflow) int {
	reverseWorkflows, accepts := invertWorkflows(workflows)
	var c int

	for _, a := range accepts {
		// Initialize an array of combinations. It contains 1 if the rating
		// combination is accepted, 0 otherwise. The first index is ignored.
		combinations := make([][]int, 4)
		for i := 0; i < 4; i++ {
			combinations[i] = make([]int, maxRating+1)
		}

		for wr, ok := a, true; ok; wr, ok = reverseWorkflows[wr.workflow] {
			w := workflows[wr.workflow]
			// Current rule has to be accepted
			updateCombinations(combinations, w[wr.rule], true)
			// All previous rules have to be rejected
			for i := wr.rule - 1; i >= 0; i-- {
				updateCombinations(combinations, w[i], false)
			}
		}

		c += countCombinations(combinations)
	}

	return c
}
