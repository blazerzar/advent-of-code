import os
from typing import List, Tuple
from math import cos, sin, radians

DIRECTIONS = [
    (1, 0), (0, 1), (-1, 0), (0, -1)
]


def move_by_instruction(instructions: List[Tuple[str, int]]) -> int:
    """ Move by given instructions and return
        Manhattan distance from the beginning """
    x, y = 0, 0
    direction = 0
    for action, value in instructions:
        if action == "N" or action == "S":
            y += value if action == "N" else -value
        elif action == "E" or action == "W":
            x += value if action == "E" else -value
        elif action == "L" or action == "R":
            direction = int(direction + (value / 90) *
                            (1 if action == "L" else -1)) % 4
        elif action == "F":
            x += DIRECTIONS[direction][0] * value
            y += DIRECTIONS[direction][1] * value
    return abs(x) + abs(y)


def move_to_waypoint(instructions: List[Tuple[str, int]]) -> int:
    """ Move towards the waypoint and return Manhattan distance
        from the beginning """
    x, y = 0, 0
    x_point, y_point = 10, 1    # Relative to the ship
    for action, value in instructions:
        if action == "N" or action == "S":
            y_point += value if action == "N" else -value
        elif action == "E" or action == "W":
            x_point += value if action == "E" else -value
        elif action == "L" or action == "R":
            degree = value if action == "L" else -value
            x_point, y_point = (round(x_point * cos(radians(degree))
                                      - y_point * sin(radians(degree))),
                                round(x_point * sin(radians(degree))
                                      + y_point * cos(radians(degree))))
        elif action == "F":
            x += value * x_point
            y += value * y_point
    return abs(x) + abs(y)


def read_file(file_name: str) -> List[Tuple[str, int]]:
    return [(line[0], int(line.strip()[1:]))for line in open(os.path.join(
        os.path.dirname(__file__), file_name), "r")]


def test():
    instructions = read_file("inputs/input12_test.txt")
    assert move_by_instruction(instructions) == 25
    assert move_to_waypoint(instructions) == 286


def main():
    test()
    instructions = read_file("inputs/input12.txt")
    print(move_by_instruction(instructions))
    print(move_to_waypoint(instructions))


if __name__ == "__main__":
    main()
