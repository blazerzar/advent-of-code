import os


def my_seat(seat_IDs: list) -> int:
    """Find the seat ID that is missing from the list"""
    min_id, max_id = min(seat_IDs), max(seat_IDs)
    seats = {id for id in range(min_id, max_id + 1)}
    return seats.difference(set(seat_IDs)).pop()


def seat_ID(boarding_pass: str) -> int:
    binary = (boarding_pass.replace("F", "0")
                           .replace("B", "1")
                           .replace("L", "0")
                           .replace("R", "1"))
    return int(binary, 2)


def read_file(file_name: str) -> list:
    return [board_pass for board_pass in [line.strip() for line in open(
            os.path.join(os.path.dirname(__file__), file_name), "r")]
            if board_pass]


def test():
    assert seat_ID("BFFFBBFRRR") == 567
    assert seat_ID("FFFBBBFRRR") == 119
    assert seat_ID("BBFFBBFRLL") == 820


def main():
    test()
    boarding_passes = read_file("inputs/input5.txt")
    seat_IDs = [seat_ID(boarding_pass) for boarding_pass in boarding_passes]
    print(max(seat_IDs))
    print(my_seat(seat_IDs))


if __name__ == "__main__":
    main()
