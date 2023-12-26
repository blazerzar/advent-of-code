package day07

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

const day = 7

// Cards from weakest to strongest (1 is placeholder)
var cardStrengths = "123456789TJQKA"

// From weakest to strongest:
//   - High card (1)
//   - One pair (2)
//   - Two pair (3)
//   - Three of a kind (4)
//   - Full house (5)
//   - Four of a kind (6)
//   - Five of a kind (7)
var handTypes = map[string]int{
	"11111": 1, "1112": 2, "122": 3, "113": 4, "23": 5, "14": 6, "5": 7,
}
var handTypesJokers = map[string]int{
	// No jokers
	"11111": 1, "1112": 2, "122": 3, "113": 4, "23": 5, "14": 6, "5": 7,
	// 1 joker
	"1111": 2, "112": 4, "22": 5, "13": 6, "4": 7,
	// 2 jokers
	"111": 4, "12": 6, "3": 7,
	// 3 jokers
	"11": 6, "2": 7,
	// 4 jokers
	"1": 7,
}

type hand struct {
	cards string
	bid   int
}

// Convert string of cards to a number representing their strength
func parseCards(cards string, jokers bool) int {
	// Jokers are replaced with 1s - not counted and the lowest strength
	if jokers {
		cards = strings.ReplaceAll(cards, "J", "1")
	}
	if cards == "11111" {
		return 70000000000
	}

	counts := make(map[rune]int)
	for _, card := range cards {
		if card == '1' {
			continue
		}
		counts[card]++
	}

	// Convert map values to sorted slice and join them into a string
	countsSlice := []int{}
	for _, count := range counts {
		countsSlice = append(countsSlice, count)
	}
	sort.Ints(countsSlice)
	countsStr := ""
	for _, count := range countsSlice {
		countsStr += strconv.Itoa(count)
	}

	var strength int
	if jokers {
		strength = handTypesJokers[countsStr]
	} else {
		strength = handTypes[countsStr]
	}

	for _, card := range cards {
		strength = 100*strength + strings.IndexRune(cardStrengths, card)
	}

	return strength
}

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath := fmt.Sprintf("inputs/day_%02d_test.txt", day)
	test(testInputPath)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	hands := readInput(inputPath)
	fmt.Println(totalWinnings(hands, false))
	fmt.Println(totalWinnings(hands, true))
}

func test(testInputPath string) {
	hands := readInput(testInputPath)
	solutions.AssertEq(totalWinnings(hands, false), 6440)
	solutions.AssertEq(totalWinnings(hands, true), 5905)
}

func readInput(path string) []hand {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	hands := []hand{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.Fields(scanner.Text())
		bid, _ := strconv.Atoi(line[1])
		hands = append(hands, hand{line[0], bid})
	}

	return hands
}

type pair struct {
	value int
	index int
}

// For each hand, multiply the bid by its ranking
func totalWinnings(hands []hand, jokers bool) int {
	pairs := []pair{}
	for i, hand := range hands {
		pairs = append(pairs, pair{parseCards(hand.cards, jokers), i})
	}
	sort.SliceStable(pairs, func(i, j int) bool {
		return pairs[i].value < pairs[j].value
	})

	winnings := 0
	for i, pair := range pairs {
		winnings += hands[pair.index].bid * (i + 1)
	}

	return winnings
}
