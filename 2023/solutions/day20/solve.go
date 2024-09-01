package day20

import (
	"aoc-2023/solutions"
	"bufio"
	"fmt"
	"os"
	"slices"
	"strings"
)

const day = 20

// Solve runs tests and solutions for specific day.
func Solve() {
	// Tests
	testInputPath1 := fmt.Sprintf("inputs/day_%02d_test_1.txt", day)
	testInputPath2 := fmt.Sprintf("inputs/day_%02d_test_2.txt", day)
	test(testInputPath1, testInputPath2)

	// Solution
	inputPath := fmt.Sprintf("inputs/day_%02d.txt", day)
	modules := readInput(inputPath)
	fmt.Println(multiplePushes(modules, 1000))
	fmt.Println(pushesForRx(modules))
}

func test(testInputPath1, testInputPath2 string) {
	modules := readInput(testInputPath1)
	solutions.AssertEq(multiplePushes(modules, 1000), 32000000)
	modules = readInput(testInputPath2)
	solutions.AssertEq(multiplePushes(modules, 1000), 11687500)
}

func readInput(path string) map[string]*module {
	file, err := os.Open(path)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	modules := make(map[string]*module)
	connections := make(map[string][]string)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		name, m := parseModule(scanner.Text())
		modules[name] = &m

		for _, dest := range m.destinations() {
			connections[dest] = append(connections[dest], name)
		}
	}

	// Fill recent values for conjunctions
	for name, m := range modules {
		if c, ok := (*m).(*conjunction); ok {
			for _, connection := range connections[name] {
				c.recent[connection] = false
			}
			*m = c
		}
	}

	return modules
}

func parseModule(line string) (string, module) {
	s := strings.Split(line, " -> ")
	destinations := moduleDestinations{strings.Split(s[1], ", ")}

	name := s[0]
	if name[0] == '%' {
		return name[1:], &flipFlop{false, destinations}
	} else if name[0] == '&' {
		// List of recent values is filled after all modules are parsed
		return name[1:], &conjunction{make(map[string]bool), 0, destinations}
	}
	return name, &broadcast{destinations}
}

type signal struct {
	pulse       bool
	origin      string
	destination string
}

// Simulate pushing a button and return the number of low and high pulses sent.
// Also check if the given signal was sent.
func pushButton(modules map[string]*module, sent signal) (int, int, bool) {
	var low, high int
	var received bool
	signals := []signal{{false, "button", "broadcaster"}}

	for len(signals) > 0 {
		s := signals[0]
		signals = signals[1:]
		if s.pulse {
			high++
		} else {
			low++
		}

		if s.origin == sent.origin && s.pulse == sent.pulse {
			received = true
		}

		receiver, ok := modules[s.destination]
		if !ok {
			continue
		}
		signals = (*receiver).receive(s, signals)
	}

	return low, high, received
}

func multiplePushes(modules map[string]*module, times int) int {
	var low, high int
	for i := 0; i < times; i++ {
		l, h, _ := pushButton(modules, signal{})
		low, high = low+l, high+h
	}
	return low * high
}

func pushesTillSignal(modules map[string]*module, signal signal) int {
	for _, m := range modules {
		(*m).reset()
	}
	for pushes := 1; ; pushes++ {
		_, _, received := pushButton(modules, signal)
		if received {
			return pushes
		}
	}
}

// Return the lowest number of button pushes to send a low pulse to rx.
func pushesForRx(modules map[string]*module) int {
	var toRx string
	for name, m := range modules {
		if slices.Contains((*m).destinations(), "rx") {
			toRx = name
			break
		}
	}

	// Conjuction module sends low pulse to rx if all recent pulses are high
	cycles := make([]int, 0)
	for name := range (*modules[toRx]).(*conjunction).recent {
		cycle := pushesTillSignal(modules, signal{true, name, ""})
		cycles = append(cycles, cycle)
	}
	return lcm(cycles)
}

type module interface {
	destinations() []string
	receive(signal, []signal) []signal
	reset()
}
type moduleDestinations struct {
	dest []string
}
type broadcast struct {
	moduleDestinations
}
type flipFlop struct {
	state bool
	moduleDestinations
}
type conjunction struct {
	recent     map[string]bool
	recentHigh int
	moduleDestinations
}

func (md *moduleDestinations) destinations() []string {
	return md.dest
}

// Re-broadcast the same signal to all destination modules.
func (b *broadcast) receive(s signal, signals []signal) []signal {
	for _, dest := range b.dest {
		signals = append(signals, signal{s.pulse, s.destination, dest})
	}
	return signals
}

// Ignore high pulse, otherwise flip the state and send the new state as a pulse.
func (f *flipFlop) receive(s signal, signals []signal) []signal {
	if !s.pulse {
		f.state = !f.state
		for _, dest := range f.dest {
			signals = append(signals, signal{f.state, s.destination, dest})
		}
	}
	return signals
}

// Remember the recent pulses and count the high ones. Send a high pulse if any
// of the recent pulses are low.
func (c *conjunction) receive(s signal, signals []signal) []signal {
	if s.pulse && !c.recent[s.origin] {
		c.recentHigh++
	} else if !s.pulse && c.recent[s.origin] {
		c.recentHigh--
	}
	c.recent[s.origin] = s.pulse
	for _, dest := range c.dest {
		s := signal{c.recentHigh != len(c.recent), s.destination, dest}
		signals = append(signals, s)
	}
	return signals
}

func (b *broadcast) reset() {}
func (f *flipFlop) reset()  { f.state = false }
func (c *conjunction) reset() {
	c.recentHigh = 0
	for k := range c.recent {
		c.recent[k] = false
	}
}

func gcd(a, b int) int {
	for b != 0 {
		a, b = b, a%b
	}
	return a
}

func lcm(nums []int) int {
	result := nums[0]
	for i := 1; i < len(nums); i++ {
		result *= nums[i] / gcd(result, nums[i])
	}
	return result
}
