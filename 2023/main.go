package main

import (
	"aoc-2023/solutions/day01"
	"aoc-2023/solutions/day02"
	"aoc-2023/solutions/day03"
	"fmt"
	"os"
	"strconv"
)

var solutionFunctions = []func(){
	nil,
	day01.Solve,
	day02.Solve,
	day03.Solve,
}

func main() {
	day, err := strconv.Atoi(os.Args[1])
	if err != nil {
		fmt.Println("Selected day is not a number.")
		os.Exit(1)
	}
	if day < 0 || day >= len(solutionFunctions) {
		fmt.Println("Selected day is out of range.")
		os.Exit(1)
	}

	solutionFunctions[day]()
}
