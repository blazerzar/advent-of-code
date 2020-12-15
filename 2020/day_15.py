from typing import List


def nth_number_spoken(starting_nums: List[int], n: int) -> int:
    """Return the n-th number that is spoken in the memory game"""
    memory = dict()
    for i, num in enumerate(starting_nums):
        memory[num] = (i + 1, i + 1)
    prev_num = starting_nums[-1]
    for i in range(len(starting_nums) + 1, n + 1):
        prev_num = memory[prev_num][1] - memory[prev_num][0]
        memory[prev_num] = ((memory[prev_num][1], i) if prev_num in memory
                            else (i, i))
    return prev_num


def test():
    assert nth_number_spoken([0, 3, 6], 2020) == 436
    assert nth_number_spoken([2, 1, 3], 2020) == 10
    assert nth_number_spoken([1, 2, 3], 2020) == 27
    assert nth_number_spoken([2, 3, 1], 2020) == 78
    assert nth_number_spoken([3, 2, 1], 2020) == 438
    assert nth_number_spoken([3, 1, 2], 2020) == 1836
    assert nth_number_spoken([0, 3, 6], 30000000) == 175594


def main():
    test()
    starting_numbers = [1, 0, 16, 5, 17, 4]
    print(nth_number_spoken(starting_numbers, 2020))
    print(nth_number_spoken(starting_numbers, 30000000))


if __name__ == "__main__":
    main()
