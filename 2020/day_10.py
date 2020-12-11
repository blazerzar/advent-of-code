import os
from typing import List


def joltage_differences(adapters: List[int]) -> int:
    """
    Return number of 1 jolt differences multiplied by number
    of 3 jolts differences
    """
    diff_1 = adapters[0] == 1
    # The difference to our device is always 3
    diff_3 = 1 + (adapters[0] == 3)
    for prev, curr in zip(adapters[:-1], adapters[1:]):
        diff_1 += curr - prev == 1
        diff_3 += curr - prev == 3
    return diff_1 * diff_3


def adapters_arrangements(adapters: List[int]) -> int:
    """Return number of valid arrangements for given adapters"""
    adapters_dict = {key: [x for x in adapters[i:i + 3] if x - key <= 3]
                     for i, key in enumerate([0] + adapters)}
    return adapters_rec(adapters_dict, 0, {adapters[-1]: 1})


def adapters_rec(adapters: dict, prev: int, memo: dict) -> int:
    """Recursive function for calculating arragements"""
    if prev in memo:
        return memo[prev]

    count = 0
    for next in adapters[prev]:
        count += adapters_rec(adapters, next, memo)
    memo[prev] = count
    return count


def adapters_dynamic(adapters: List[int]) -> int:
    """Dynamic programming approach for second part"""
    results = dict()
    for i, a in enumerate(adapters):
        # Find possible previous adapters
        prev = [x for j, x in enumerate(
            adapters[i::-1]) if 0 < j <= 3 and a - x <= 3]
        results[a] = sum([results[j] for j in prev]) + int(a <= 3)
    return results[adapters[-1]]


def read_file(file_name: str) -> List[int]:
    return sorted([int(line) for line in open(os.path.join(
        os.path.dirname(__file__), file_name), "r")])


def test():
    adapters_1 = read_file("inputs/input10_test1.txt")
    adapters_2 = read_file("inputs/input10_test2.txt")
    assert joltage_differences(adapters_1) == 35
    assert joltage_differences(adapters_2) == 220
    assert adapters_arrangements(adapters_1) == 8
    assert adapters_arrangements(adapters_2) == 19208


def main():
    test()
    adapters = read_file("inputs/input10.txt")
    print(joltage_differences(adapters))
    print(adapters_arrangements(adapters))
    print(adapters_dynamic(adapters))


if __name__ == "__main__":
    main()
