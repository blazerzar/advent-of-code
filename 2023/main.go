package main

import (
	"aoc-2023/solutions/day01"
	"aoc-2023/solutions/day02"
	"aoc-2023/solutions/day03"
	"aoc-2023/solutions/day04"
	"aoc-2023/solutions/day05"
	"aoc-2023/solutions/day06"
	"aoc-2023/solutions/day07"
	"aoc-2023/solutions/day08"
	"aoc-2023/solutions/day09"
	"aoc-2023/solutions/day10"
	"aoc-2023/solutions/day11"
	"aoc-2023/solutions/day12"
	"aoc-2023/solutions/day13"
	"aoc-2023/solutions/day14"
	"aoc-2023/solutions/day15"
	"aoc-2023/solutions/day16"
	"aoc-2023/solutions/day17"
	"aoc-2023/solutions/day18"
	"aoc-2023/solutions/day19"
	"aoc-2023/solutions/day20"
	"aoc-2023/solutions/day21"
	"aoc-2023/solutions/day22"
	"aoc-2023/solutions/day23"
	"fmt"
	"os"
	"strconv"
)

var solutionFunctions = []func(){
	nil,
	day01.Solve,
	day02.Solve,
	day03.Solve,
	day04.Solve,
	day05.Solve,
	day06.Solve,
	day07.Solve,
	day08.Solve,
	day09.Solve,
	day10.Solve,
	day11.Solve,
	day12.Solve,
	day13.Solve,
	day14.Solve,
	day15.Solve,
	day16.Solve,
	day17.Solve,
	day18.Solve,
	day19.Solve,
	day20.Solve,
	day21.Solve,
	day22.Solve,
	day23.Solve,
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
