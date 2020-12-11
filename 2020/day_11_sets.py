import os
from typing import Set, Callable, Tuple

DIRECTIONS = [
    (-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)
]


def simulate_seats(seats: Set[Tuple[int, int]], limit: int,
                   counter: Callable[[Set[Tuple[int, int]],
                                      Set[Tuple[int, int]],
                                      int, int], int]) -> int:
    """
    Simulate seat states changes until stable and return
    number of occupied seats
    """
    taken = set()
    empty = {s for s in seats}
    changes = set()
    max_row, max_column = max(x[0] for x in seats), max(x[1] for x in seats)
    while True:
        changes = set()

        for s in empty:
            args = ((taken, *s) if limit == 4
                    else (seats, taken, *s, max_row, max_column))
            if counter(*args) == 0:
                changes.add(s)

        for s in taken:
            args = ((taken, *s) if limit == 4
                    else (seats, taken, *s, max_row, max_column))
            if counter(*args) >= limit:
                changes.add(s)

        if not changes:
            break

        taken = taken.symmetric_difference(changes)
        empty = empty.symmetric_difference(changes)
        changes = set()
    return len(taken)


def num_of_occupied(taken: Set[Tuple[int, int]],
                    row: int, column: int) -> int:
    """Return number of occupied seats around the given one"""
    count = 0
    for dy, dx in DIRECTIONS:
        count += (row + dy, column + dx) in taken
    return count


def num_of_visible(seats: Set[Tuple[int, int]], taken: Set[Tuple[int, int]],
                   row: int, column: int,
                   max_row: int, max_column: int) -> int:
    """Return number of visible seats from the given one that are taken"""
    count = 0
    for dy, dx in DIRECTIONS:
        new_row, new_column = row + dy, column + dx
        while 0 <= new_row <= max_row and 0 <= new_column <= max_column:
            count += (new_row, new_column) in taken
            if (new_row, new_column) in seats:
                break
            new_row += dy
            new_column += dx
    return count


def read_file(file_name: str) -> Set[Tuple[int, int]]:
    return {(y, x) for y, line in enumerate(open(os.path.join(
        os.path.dirname(__file__), file_name), "r"))
        for x, c in enumerate(line.strip()) if c == "L"}


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
