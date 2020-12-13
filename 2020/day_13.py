import os
from typing import List, Tuple
from math import gcd, ceil


def first_bus(timestamp: int, buses: List[Tuple[int, int]]) -> int:
    """ Return ID of the earliest bus you can take
        multiplied by the number of minutes to wait """
    earliest_bus = min([(bus_id, bus_id - timestamp % bus_id)
                        for _, bus_id in buses], key=lambda x: x[1])
    return earliest_bus[0] * earliest_bus[1]


def all_buses_depart(buses: List[Tuple[int, int]]) -> int:
    """ Return timestamp where all buses depart based on their
        postion in the list """
    a = buses[0][1]
    b = 0
    prev = 0
    for offset, num in buses[1:]:
        res = euclid_coefficients(a, num, offset + b)
        prev = res
        print(res, a, b)
        if num == buses[-1][1]:
            break
        b = a * res[0]
        a = lcm(a, num)
    print(b + a * prev[0])
    return b + a * prev[0]


def lcm(a: int, b: int) -> int:
    return abs(a) * abs(b) // gcd(a, b)


def euclid_coefficients(a: int, b: int, result: int) -> int:
    """ Return coefficients x and y so -x * a + y * b = result """
    reversed_nums = False
    if b > a:
        a, b = b, a
        reversed_nums = True
    x1, y1 = 1, 0   # x1 * a + y1 * b = z1
    x2, y2 = 0, 1   # x2 * a + y2 * b = z2

    while x2 * a + y2 * b != 1:
        mult = (x1 * a + y1 * b) // (x2 * a + y2 * b)
        x1, y1, x2, y2 = x2, y2, x1 - mult * x2, y1 - mult * y2

        if x2 * a + y2 * b == 0:
            factor = result // (x1 * a + y1 * b)
            return ((abs(x1 * factor), (y1 * factor)) if not reversed_nums
                    else (abs(y1 * factor), (x1 * factor)))

    x2, y2 = x2 * result, y2 * result
    if x2 > 0 and not reversed_nums or y2 > 0 and reversed_nums:
        factor = ceil(x2 / b if not reversed_nums else y2 / a)
        return ((abs(x2 - factor * b), y2 + factor * a) if not reversed_nums
                else (abs(y2 - factor * a), x2 + factor * b))
    return (abs(x2), y2) if not reversed_nums else (abs(y2), x2)


def read_file(file_name: str) -> Tuple[int, List[Tuple[int, int]]]:
    lines = open(os.path.join(
        os.path.dirname(__file__), file_name), "r").readlines()
    return (int(lines[0].strip()),
            [(i, int(bus_id)) for i, bus_id in
             enumerate(lines[1].strip().split(",")) if bus_id != "x"])


def test():
    timestamp, buses = read_file("inputs/input13_test.txt")
    assert first_bus(timestamp, buses) == 295
    assert lcm(124, 234) == 14508
    assert euclid_coefficients(17, 13, 2) == (6, 8)
    assert euclid_coefficients(17, 19, 4) == (2, 2)
    assert euclid_coefficients(13, 19, 7) == (17, 12)
    all_buses_depart([(0, 17), (2, 13), (3, 19)])
    assert all_buses_depart(buses) == 1068781


def main():
    test()
    timestamp, buses = read_file("inputs/input13.txt")
    print(first_bus(timestamp, buses))
    # print(all_buses_depart(buses))


if __name__ == "__main__":
    main()
