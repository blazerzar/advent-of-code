import functools
import os
from typing import Tuple


def num_of_trees(map_data: list) -> Tuple[int, int]:
    """Return number of trees in one direction and in all multiplied"""
    counts = [0] * 5
    offsets = [(1, 1), (3, 1), (5, 1), (7, 1), (1, 2)]
    current_index = [0] * 5
    for i, line in enumerate(map_data):
        counts = [count + (not i % move[1] and line[index] == "#")
                  for count, index, move
                  in zip(counts, current_index, offsets)]
        current_index = [
            (current + (move[0] if not i % move[1] else 0)) % len(line)
            for current, move in zip(current_index, offsets)]
    return counts[1], functools.reduce(lambda r, n: r * n, counts, 1)


def read_file(file_name: str) -> list:
    return [line[:-1] for line in
            open(os.path.join(os.path.dirname(__file__), file_name), "r")]


def test():
    map_data = read_file("inputs/input3_test.txt")
    trees_part_1, trees_part_2 = num_of_trees(map_data)
    assert trees_part_1 == 7
    assert trees_part_2 == 336


def main():
    test()
    map_data = read_file("inputs/input3.txt")
    print(num_of_trees(map_data))


if __name__ == "__main__":
    main()
