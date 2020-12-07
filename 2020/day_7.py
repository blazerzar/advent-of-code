import os
import re
from typing import Tuple


def bags_that_hold(rules: dict, bag: str) -> set:
    """Return the number of bags that can hold given bag"""
    bags = set()
    for key, value in rules.items():
        if value.keys() and bag in value.keys():
            bags.add(key)
            # Check recursively which bags can hold the found bag
            bags |= bags_that_hold(rules, key)
    return bags


def num_of_subbags(rules: dict, bag: str) -> int:
    """Return the number of empty bags that are needed to carry given bag"""
    count = 0
    # Check which bags we need to carry directly
    bags = rules[bag]
    for b, quantity in bags.items():
        # We need to carry quantity of b bags
        count += quantity
        # Each b bag contains num_of_subbags(rules, b) bags
        count += num_of_subbags(rules, b) * quantity
    return count


def read_file(file__name: str) -> list:
    """
    Create a dictionary with rules, key is the bag color, value is
    a dictionary that contains possible bags and their max number
    """
    rules = {}
    with open(os.path.join(os.path.dirname(__file__), file__name), "r") as f:
        for line in f:
            match = re.fullmatch(
                r"(([\w|\s]+) bags contain (\d+.*)\.)|(([\w|\s]+) bags .+)",
                line.strip())
            if match and match.group(1):
                rules[match.group(2)] = {key: value for key, value in [
                    get_num_and_color(rule.strip()) for rule in
                    match.group(3).split(", ")]}
            elif match and match.group(4):
                # Mark that this bag contains no other bag
                rules[match.group(5)] = dict()
    return rules


def get_num_and_color(rule: str) -> Tuple[str, int]:
    """Get the bag color and quantity from one rule"""
    match = re.match(r"(\d+) ([\w|\s]+?) bags?", rule)
    if match:
        return match.group(2), int(match.group(1))


def test():
    rules = read_file("inputs/input7_test1.txt")
    assert len(bags_that_hold(rules, "shiny gold")) == 4
    rules = read_file("inputs/input7_test2.txt")
    assert num_of_subbags(rules, "shiny gold") == 126


def main():
    test()
    rules = read_file("inputs/input7.txt")
    print(len(bags_that_hold(rules, "shiny gold")))
    print(num_of_subbags(rules, "shiny gold"))


if __name__ == "__main__":
    main()
