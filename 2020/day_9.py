import os
from typing import List


def first_num_no_sum(numbers: List[int], preamble: int) -> int:
    """
    Return the first number that cannot be writen as a sum of two
    <preamble> numbers before it
    """
    available = numbers[:preamble]
    for num in numbers[preamble:]:
        for n in available:
            if num - n in available:
                break
        else:
            return num
        available.pop(0)
        available.append(num)


def set_with_sum(numbers: List[int], num: int) -> int:
    """
    Find a contiguous set of at least two numbers that add to <sum>
    and return sum of smaller and largest numbers in the set
    """
    for i, n1 in enumerate(numbers[:-1]):
        sum = n1
        min_num = n1
        max_num = n1
        for n2 in numbers[i + 1:]:
            sum += n2
            min_num = min_num if n2 > min_num else n2
            max_num = max_num if n2 < max_num else n2
            if sum == num:
                return min_num + max_num
            elif sum > num:
                break


def read_file(file_name: str) -> List[int]:
    return [int(line.strip()) for line in open(os.path.join(
            os.path.dirname(__file__), file_name), "r") if line.strip()]


def test():
    numbers = read_file("inputs/input9_test.txt")
    assert first_num_no_sum(numbers, 5) == 127
    assert set_with_sum(numbers, 127) == 62


def main():
    test()
    numbers = read_file("inputs/input9.txt")
    invalid_number = first_num_no_sum(numbers, 25)
    print(invalid_number)
    print(set_with_sum(numbers, invalid_number))


if __name__ == "__main__":
    main()
