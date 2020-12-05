import os


def part1(numbers: list, input_sum: int) -> int:
    """Return product of two numbers in <numbers> that sum to <input_sum>"""
    numbers_set = set(numbers)
    for n in numbers:
        if input_sum - n in numbers_set:
            return n * (input_sum - n)


def part2(numbers: list, input_sum: int) -> int:
    """Return product of three numbers in <numbers> that sum to <input_sum>"""
    numbers_set = set(numbers)
    for n1 in numbers:
        for n2 in numbers:
            if n1 != n2 and input_sum - n1 - n2 in numbers_set:
                return n1 * n2 * (input_sum - n1 - n2)


def read_file(file_name: str) -> list:
    return [int(line) for line in
            open(os.path.join(os.path.dirname(__file__), file_name), "r")]


def test():
    numbers = read_file("inputs/input1_test.txt")
    input_sum = 2020
    assert part1(numbers, input_sum) == 514579
    assert part2(numbers, input_sum) == 241861950


def main():
    test()
    input_sum = 2020
    numbers = read_file("inputs/input1.txt")
    print(part1(numbers, input_sum))
    print(part2(numbers, input_sum))


if __name__ == "__main__":
    main()
