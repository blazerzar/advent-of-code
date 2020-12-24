import os
import re
from typing import List, Tuple, Set, Dict
from collections import defaultdict

NEIGHBOURS = [
    (1, 0), (1, -1), (0, -1), (-1, 0), (-1, 1), (0, 1)
]


def coords_from_instructions(instruction: List[str]) -> Tuple[int, int]:
    """Return coordinates of the tile we get by following the instructions"""
    x, y = 0, 0
    for d in instruction:
        x += d == "e" or d == "se"
        x -= d == "w" or d == "nw"
        y += d == "ne" or d == "nw"
        y -= d == "se" or d == "sw"
    return x, y


def find_black_tiles(tiles: List[List[str]]) -> List[Tuple[int, int]]:
    """Return a list of coordinates indentifying black tiles"""
    blacks = set()
    for tile in tiles:
        blacks = blacks.symmetric_difference({coords_from_instructions(tile)})
    return blacks


def get_whites_neighbours(blacks: Set[Tuple[int, int]]) -> Dict[
        Tuple[int, int], int]:
    """Return white tiles that neighbour at least 1 black tile
    and number of their black neighbours
    """
    whites = defaultdict(int)
    for x, y in blacks:
        for dx, dy in NEIGHBOURS:
            whites[(x + dx, y + dy)] += (x + dx, y + dy) not in blacks
    return whites


def get_black_neighbours(blacks: Set[Tuple[int, int]], x: int, y: int) -> int:
    """Return number of black tiles around tile (x, y)"""
    count = 0
    for dx, dy in NEIGHBOURS:
        count += (x + dx, y + dy) in blacks
    return count


def simulate_days(start_blacks: Set[Tuple[int, int]], days: int) -> int:
    """Return number of black tiles after <days> days"""
    blacks = start_blacks.copy()

    for i in range(days):
        changed = set()

        for black_x, black_y in blacks:
            if get_black_neighbours(blacks, black_x, black_y) not in {1, 2}:
                changed.add((black_x, black_y))

        for (white_x, white_y), count in get_whites_neighbours(blacks).items():
            if count == 2:
                changed.add((white_x, white_y))

        blacks = blacks.symmetric_difference(changed)
    return len(blacks)


def read_file(file_name: str) -> List[List[str]]:
    return [re.findall("e|w|se|sw|ne|nw", line) for line in open(os.path.join(
        os.path.dirname(__file__), file_name), "r")]


def test():
    tiles = read_file("inputs/input24_test.txt")
    assert coords_from_instructions(["nw", "w", "sw", "e", "e"]) == (0, 0)
    blacks = find_black_tiles(tiles)
    assert len(blacks) == 10
    assert simulate_days(blacks, 100) == 2208


def main():
    test()
    tiles = read_file("inputs/input24.txt")
    blacks = find_black_tiles(tiles)
    print(len(blacks))
    print(simulate_days(blacks, 100))


if __name__ == "__main__":
    main()
