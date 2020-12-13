import os
from typing import List, Tuple
from math import gcd
from functools import reduce


def first_bus(timestamp: int, buses: List[Tuple[int, int]]) -> int:
    """Return ID of the earliest bus you can take
       multiplied by the number of minutes to wait"""
    earliest_bus = min([(bus_id, bus_id - timestamp % bus_id)
                        for _, bus_id in buses], key=lambda x: x[1])
    return earliest_bus[0] * earliest_bus[1]


def all_buses_depart(buses: List[Tuple[int, int]]) -> int:
    """Return timestamp where all buses depart based on their
       position in the list"""
    x = 0
    lcm_of_all = reduce(lcm, [bus_id for _, bus_id in buses])
    for offset, bus in buses:
        x += ((bus - offset) * (lcm_of_all // bus)
              * modular_inverse(lcm_of_all // bus, bus))
    return x % lcm_of_all


def modular_inverse(a: int, m: int) -> int:
    # a * <modular_inverse> = 1 (mod m)
    x1, y1 = 1, 0
    x2, y2 = 0, 1
    while x2 * m + y2 * a != 0:
        mult = (x1 * m + y1 * a) // (x2 * m + y2 * a)
        x1, y1, x2, y2 = x2, y2, x1 - mult * x2, y1 - mult * y2
    return y1 if y1 > 0 else y1 + m


def lcm(a: int, b: int) -> int:
    return abs(a) * abs(b) // gcd(a, b)


def read_file(file_name: str) -> Tuple[int, List[Tuple[int, int]]]:
    lines = open(os.path.join(
        os.path.dirname(__file__), file_name), "r").readlines()
    return (int(lines[0].strip()),
            [(i, int(bus_id)) for i, bus_id in
             enumerate(lines[1].strip().split(",")) if bus_id != "x"])


def test():
    timestamp, buses = read_file("inputs/input13_test.txt")
    assert first_bus(timestamp, buses) == 295
    assert modular_inverse(43, 600) == 307
    assert lcm(124, 234) == 14508
    assert all_buses_depart([(0, 17), (2, 13), (3, 19)]) == 3417
    assert all_buses_depart([(0, 67), (1, 7), (2, 59), (3, 61)]) == 754018
    assert all_buses_depart([(0, 67), (2, 7), (3, 59), (4, 61)]) == 779210
    assert all_buses_depart([(0, 67), (1, 7), (3, 59), (4, 61)]) == 1261476
    assert all_buses_depart(
        [(0, 1789), (1, 37), (2, 47), (3, 1889)]) == 1202161486
    assert all_buses_depart(buses) == 1068781


def main():
    test()
    timestamp, buses = read_file("inputs/input13.txt")
    print(first_bus(timestamp, buses))
    print(all_buses_depart(buses))


if __name__ == "__main__":
    main()
