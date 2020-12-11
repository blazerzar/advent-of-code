import os
import copy
from typing import List, Callable

DIRECTIONS = [
    (1, -1), (1, 0), (1, 1), (0, -1), (0, 1), (-1, -1), (-1, 0), (-1, 1)
]


def simulate_seats(seats_og: List[List[int]], limit: int,
                   num_occ: Callable[[List[List[int]], int, int], int]) -> int:
    """Simlulate seat states changing until stable"""
    seats = copy.deepcopy(seats_og)
    changed = [(0, 0)]
    occ_seats = 0
    while changed:
        changed = list()
        for i, row in enumerate(seats):
            for j, c in enumerate(row):
                if c == ".":
                    continue
                occupied = num_occ(seats, i, j)
                if (c == "L" and occupied == 0
                        or c == "#" and occupied >= limit):
                    changed.append((i, j))

        # Change seat states
        for i, j in changed:
            if seats[i][j] == "L":
                occ_seats += 1
                seats[i][j] = "#"
            else:
                occ_seats -= 1
                seats[i][j] = "L"
    return occ_seats


def num_of_occupied(seats: List[List[int]], row: int, column: int) -> int:
    """Return number of occupied neighbouring seats for given seat"""
    count = 0
    for dy, dx in DIRECTIONS:
        new_row, new_column = row + dy, column + dx
        if 0 <= new_row < len(seats) and 0 <= new_column < len(seats[new_row]):
            count += seats[new_row][new_column] == "#"
    return count


def num_of_visible(seats: List[List[int]], row: int, column: int) -> int:
    """Return number of visibly occupied seats from given seat"""
    count = 0
    for dy, dx in DIRECTIONS:
        new_row, new_column = row + dy, column + dx
        while (0 <= new_row < len(seats)
               and 0 <= new_column < len(seats[new_row])):
            if seats[new_row][new_column] == "L":
                break
            elif seats[new_row][new_column] == "#":
                count += 1
                break
            new_row += dy
            new_column += dx
    return count


def read_file(file_name: str) -> List[List[str]]:
    return [list(line.strip()) for line in open(
        os.path.join(os.path.dirname(__file__), file_name), "r")]


def test():
    seats = read_file("inputs/input11_test.txt")
    assert simulate_seats(seats, 4, num_of_occupied) == 37
    assert simulate_seats(seats, 5, num_of_visible) == 26


def main():
    test()
    seats = read_file("inputs/input11.txt")
    print(simulate_seats(seats, 4, num_of_occupied))
    print(simulate_seats(seats, 5, num_of_visible))


if __name__ == "__main__":
    main()
