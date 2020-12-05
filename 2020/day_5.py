import math
import os


def my_seat(seat_IDs: list) -> int:
    """Find the seat ID that is missing from the list"""
    min_id, max_id = min(seat_IDs), max(seat_IDs)
    seats = {id for id in range(min_id, max_id + 1)}
    return seats.difference(set(seat_IDs)).pop()


def seat_location(boarding_pass: str) -> tuple:
    """Calculate seat row and column from a boarding pass"""
    row_min, row_max = 0, 127
    column_min, column_max = 0, 7
    for i, c in enumerate(boarding_pass):
        if i < 7:
            if c == "F":
                row_max = row_min + int((row_max - row_min) / 2)
            elif c == "B":
                row_min = row_min + math.ceil((row_max - row_min) / 2)
        else:
            if c == "L":
                column_max = column_min + int((column_max - column_min) / 2)
            elif c == "R":
                column_min = column_min + math.ceil(
                    (column_max - column_min) / 2)
    return row_min, column_min


def seat_ID(row: int, column: int) -> int:
    return row * 8 + column


def read_file(file_name: str) -> list:
    return [board_pass for board_pass in [line.strip() for line in open(
            os.path.join(os.path.dirname(__file__), file_name), "r")]
            if board_pass]


def test():
    assert seat_location("FBFBBFFRLR") == (44, 5)
    assert seat_location("BFFFBBFRRR") == (70, 7)
    assert seat_location("FFFBBBFRRR") == (14, 7)
    assert seat_location("BBFFBBFRLL") == (102, 4)
    assert seat_ID(44, 5) == 357
    assert seat_ID(70, 7) == 567
    assert seat_ID(14, 7) == 119
    assert seat_ID(102, 4) == 820


def main():
    test()
    boarding_passes = read_file("inputs/input5.txt")
    seat_IDs = [seat_ID(*seat_location(boarding_pass))
                for boarding_pass in boarding_passes]
    print(max(seat_IDs))
    print(my_seat(seat_IDs))


if __name__ == "__main__":
    main()
