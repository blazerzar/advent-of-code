import os
from typing import Tuple

def passwords_count(passwords: tuple) -> Tuple[int, int]:
    """Return number of good passwords for 1st and 2nd part"""
    count_old, count_new = 0, 0
    for data in passwords:
        limits, char, password = data
        first, last = (int(x) for x in limits.split("-"))
        char = char[:-1]

        count_old += first <= password.count(char) <= last
        count_new += (password[first - 1]
                      + password[last - 1]).count(char) == 1
    return count_old, count_new


def read_file(file_name: str) -> tuple:
    return (line.split() for line in
            open(os.path.join(os.path.dirname(__file__), file_name), "r"))


def test():
    passwords = read_file("inputs/input2_test.txt")
    count_old, count_new = passwords_count(passwords)
    assert count_old == 2
    assert count_new == 1


def main():
    test()
    passwords = read_file("inputs/input2.txt")
    print(passwords_count(passwords))


if __name__ == "__main__":
    main()
