import os
from typing import Tuple


def read_file(file_name: str) -> Tuple[list, list]:
    """
    Read file and create two lists of sets, one contains answers answered by
    at least one person and the other by everyone
    """
    groups = [line.split() for line in open(os.path.join(
              os.path.dirname(__file__), file_name), "r")
              .read().split("\n\n")]
    answers_part1 = [set.union(*(set(a) for a in g)) for g in groups]
    answers_part2 = [set.intersection(*(set(a) for a in g)) for g in groups]
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
