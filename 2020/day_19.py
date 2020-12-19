import os
from typing import Tuple, List, Set


def create_rule(rule: int, rules: dict) -> Set[str]:
    if isinstance(rules[rule], str):
        return {rules[rule]}

    result = set()
    for r in rules[rule]:
        result = result.union(concat_rules(r, rules))
    return result


def concat_rules(nums: List[int], rules: dict) -> Set[str]:
    """Concat rules of given nums by creating all possible combinations"""
    if len(nums) == 1:
        return create_rule(nums[0], rules)

    first = create_rule(nums[0], rules)
    other = concat_rules(nums[1:], rules)
    result = set()
    for f in first:
        for o in other:
            result.add(f + o)
    return result


def num_of_cyclic(messages: List[str], rules: dict) -> int:
    """Count messages that match cyclic rules 8 and 11"""
    language_8 = create_rule(8, rules)
    language_11 = create_rule(11, rules)
    language_42 = create_rule(42, rules)
    language_31 = create_rule(31, rules)

    # Get lengths of words in languages of 8 and 11
    len_8, len_11, len_42, len_31 = (len(next(iter(language_8))),
                                     len(next(iter(language_11))),
                                     len(next(iter(language_42))),
                                     len(next(iter(language_31))))
    count = 0

    for msg in messages:
        for part8, part11 in [(msg[0:i], msg[i:])
                              for i in range(len_8, len(msg), len_8)]:
            if not (len(part11) - len_11) % (len_42 + len_31):
                count += (is_8_cyclic(part8, language_8, len_8) and
                          is_11_cyclic(part11, language_11, len_11,
                                       language_42, len_42,
                                       language_31, len_31))
    return count


def is_8_cyclic(msg: str, language_8: Set[str], len_8: int) -> bool:
    """Return True if <msg> is a cyclic message for rule 8"""
    for part in [msg[i:i + len_8] for i in range(0, len(msg), len_8)]:
        if part not in language_8:
            return False
    return True


def is_42_cyclic(msg: str, language_42: Set[str], len_42: int) -> bool:
    """Return True if <msg> is a cyclic message for rule 42"""
    for part in [msg[i:i + len_42] for i in range(0, len(msg), len_42)]:
        if part not in language_42:
            return False
    return True


def is_31_cyclic(msg: str, language_31: Set[str], len_31: int) -> bool:
    """Return True if <msg> is a cyclic message for rule 31"""
    for part in [msg[i:i + len_31] for i in range(0, len(msg), len_31)]:
        if part not in language_31:
            return False
    return True


def is_11_cyclic(msg: str, language_11: Set[str], len_11: int,
                 language_42: Set[str], len_42: int,
                 language_31: Set[str], len_31: int) -> bool:
    """Return True if <msg> is a cyclic message for rule 11"""
    i = len(msg) // 2 - len_11 // 2
    j = i + len_11
    part42 = msg[0:i]
    part11 = msg[i:j]
    part31 = msg[j:]
    if (is_42_cyclic(part42, language_42, len_42) and part11 in language_11
            and is_31_cyclic(part31, language_31, len_31)):
        return True
    return False


def read_file(file_name: str) -> Tuple[dict, List[str]]:
    input_array = open(os.path.join(
        os.path.dirname(__file__), file_name), "r").read().split("\n\n")
    rules = dict()
    for key, rule in [rule.split(":") for rule in input_array[0].split("\n")]:
        if rule.strip()[0] == '"' and rule.strip()[-1] == '"':
            rules[int(key)] = rule.strip()[1:2]
        else:
            rules[int(key)] = list()
            for r in rule.split("|"):
                rules[int(key)].append([int(x) for x in r.strip().split(" ")])

    messages = [line.strip() for line in input_array[1].split("\n")]
    return rules, messages


def test():
    rules, messages = read_file("inputs/input19_test1.txt")
    language = create_rule(0, rules)
    assert len("".join(
        ["." if msg in language else "" for msg in messages])) == 2
    rules, messages = read_file("inputs/input19_test2.txt")
    language = create_rule(0, rules)
    assert num_of_cyclic(messages, rules) == 12


def main():
    test()
    rules, messages = read_file("inputs/input19.txt")
    language = create_rule(0, rules)
    matches = len("".join(
        ["." if msg in language else "" for msg in messages]))
    print(matches)

    cyclic_matches = num_of_cyclic(messages, rules)
    print(cyclic_matches)


if __name__ == "__main__":
    main()
