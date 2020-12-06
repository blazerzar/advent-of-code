import os
from typing import Tuple


def read_file(file_name: str) -> Tuple[list, list]:
    """
    Read file and create two lists of sets, one contains answers answered by
    at least one person and the other by everyone
    """
    answers_part1 = []
    answers_part2 = []
    index = 0
    with open(os.path.join(os.path.dirname(__file__), file_name), "r") as f:
        for line in f:
            answers = {c for c in line.strip()}
            if not answers:
                index += 1
            elif answers and len(answers_part1) <= index:
                answers_part1.append(set())
                answers_part2.append(answers)
            if answers:
                answers_part1[-1] |= answers
                answers_part2[-1] &= answers
    return answers_part1, answers_part2


def test():
    answers_part1, answers_part2 = read_file("inputs/input6_test.txt")
    assert sum(len(a) for a in answers_part1) == 11
    assert sum(len(a) for a in answers_part2) == 6


def main():
    test()
    answers_part1, answers_part2 = read_file("inputs/input6.txt")
    print(sum(len(a) for a in answers_part1))
    print(sum(len(a) for a in answers_part2))


if __name__ == "__main__":
    main()
