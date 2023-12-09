package day05

import (
	"aoc-2023/solutions"
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
)

const day = 5

type valRange struct {
	start  int64
	length int64
}

type almanac struct {
	maps []mapping
}

type mapping struct {
	destination int64
	sourceRange valRange
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	seeds, almanacs := readInput(inputPath)
	fmt.Println(part1(seeds, almanacs))
	fmt.Println(part2(seeds, almanacs))
}

func test(testInputPath string) {
	testSeeds, testAlmanacs := readInput(testInputPath)
	solutions.AssertEq(part1(testSeeds, testAlmanacs), 35)
	solutions.AssertEq(part2(testSeeds, testAlmanacs), 46)
}

func readInput(path string) ([]int64, []almanac) {
	dat, err := os.ReadFile(path)
	if err != nil {
		panic(err)
	}

	seeds := make([]int64, 0)
	almanacs := make([]almanac, 0)

	sections := strings.Split(strings.TrimSpace(string(dat)), "\n\n")

	// Parse seeds
	for _, s := range strings.Fields(strings.Split(sections[0], ": ")[1]) {
		num, _ := strconv.ParseInt(s, 10, 64)
		seeds = append(seeds, num)
	}

	// Parse almanacs
	for _, a := range sections[1:] {
		lines := strings.Split(a, "\n")
		a := almanac{maps: make([]mapping, 0)}

		for _, l := range lines[1:] {
			m := strings.Fields(l)
			destinationStart, _ := strconv.ParseInt(m[0], 10, 64)
			sourceStart, _ := strconv.ParseInt(m[1], 10, 64)
			rangeLength, _ := strconv.ParseInt(m[2], 10, 64)
			a.maps = append(a.maps, mapping{
				destination: destinationStart,
				sourceRange: valRange{
					start:  sourceStart,
					length: rangeLength,
				},
			})
		}

		almanacs = append(almanacs, a)
	}

	return seeds, almanacs
}

func part1(seeds []int64, almanacs []almanac) int64 {
	return getLowestLocation(seeds, almanacs, 1)
}

func part2(seeds []int64, almanacs []almanac) int64 {
	return getLowestLocation(seeds, almanacs, 2)
}

func getLowestLocation(seeds []int64, almanacs []almanac, part int) int64 {
	// Create seed ranges
	seedRanges := make([]valRange, 0)
	for i := 0; i < len(seeds); i += part {
		var length int64 = 1
		if part == 2 {
			length = seeds[i+1]
		}
		seedRanges = append(seedRanges, valRange{
			start:  seeds[i],
			length: length,
		})
	}

	// Apply almanacs
	for _, a := range almanacs {
		seedRanges = updateRanges(seedRanges, a.maps)
	}

	lowest := seedRanges[0].start
	for _, r := range seedRanges {
		if r.start < lowest {
			lowest = r.start
		}
	}

	return lowest
}

func updateRanges(ranges []valRange, maps []mapping) []valRange {
	updated := make([]valRange, 0)

	for _, m := range maps {
		i := 0
		for i < len(ranges) {
			r := ranges[i]
			// Check if range is affected by mapping
			if r.start >= m.sourceRange.start+m.sourceRange.length {
				i++
				continue
			}
			if r.start+r.length <= m.sourceRange.start {
				i++
				continue
			}

			// Get affected area
			first := int64(math.Max(
				float64(r.start), float64(m.sourceRange.start)))
			last := int64(math.Min(
				float64(r.start+r.length-1),
				float64(m.sourceRange.start+m.sourceRange.length-1),
			))
			updated = append(updated, valRange{
				start:  m.destination + first - m.sourceRange.start,
				length: last - first + 1,
			})

			// Remove old range
			if i < len(ranges)-1 {
				ranges = append(ranges[:i], ranges[i+1:]...)
			} else {
				ranges = ranges[:i]
			}

			// Add unaffected area
			if r.start < first {
				ranges = append(ranges, valRange{
					start: r.start, length: first - r.start})
			}
			if r.start+r.length-1 > last {
				ranges = append(ranges, valRange{
					start: last + 1, length: r.start + r.length - last - 1})
			}

		}
	}

	return append(ranges, updated...)
}
