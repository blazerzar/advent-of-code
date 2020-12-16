import os
from typing import Tuple, List, Set
import re
from math import prod


def get_invalid_tickets(rules: List[Tuple[str, Set[int]]],
                        tickets: List[Tuple[int, ...]]) -> int:
    """Return number of tickets that have invalid values"""
    whole_range = set.union(*(s for _, s in rules))
    count = 0

    invalid_tickets = set()

    for ticket in tickets:
        for t in ticket:
            if t not in whole_range:
                count += t
                if ticket not in invalid_tickets:
                    invalid_tickets.add(ticket)

    # Remove invalid tickets
    for invalid in invalid_tickets:
        tickets.remove(invalid)
    return count


def get_fields_order(rules: List[Tuple[str, Set[int]]],
                     tickets: List[Tuple[int, ...]]) -> Tuple[str, ...]:
    """Return a tuple of orders fields"""
    possible_fields = []
    # Go through each field and determine possible fields
    for i in range(len(rules)):
        field_union = {t[i] for t in tickets}
        possible_fields.append(
            {field for field, values in rules if field_union <= values})

    # Remove determined fields from others
    determined_fields = set()
    while (len(determined_fields)) < len(rules):
        for i, possible in enumerate(possible_fields):
            possible_fields[i] = possible.difference(
                determined_fields) or possible
            if len(possible_fields[i]) == 1:
                determined_fields.add(*possible_fields[i])

    return tuple(field.pop() for field in possible_fields)


def get_departure_values(my_ticket: Tuple[int, ...], order: Tuple[str]) -> int:
    """Return product of values in departure fields"""
    return prod(value for field, value in zip(
        order, my_ticket) if field.startswith("departure"))


def read_file(file_name: str) -> Tuple[List[Tuple[str, Set[int]]],
                                       Tuple[int, ...], List[Tuple[int, ...]]]:
    rules = []
    input_data = open(os.path.join(os.path.dirname(__file__),
                                   file_name), "r").read().split("\n\n")
    for r in input_data[0].split("\n"):
        match = re.match(r"(.*): (\d+)-(\d+) or (\d+)-(\d+)", r)
        rules.append((match.group(1), set.union(
            {i for i in range(int(match.group(2)), int(match.group(3)) + 1)},
            {i for i in range(int(match.group(4)), int(match.group(5)) + 1)})))

    my_ticket = tuple(int(a) for a in input_data[1].split("\n")[1].split(","))
    tickets = [tuple(int(a) for a in t.split(","))
               for t in input_data[2].split("\n")[1:]]
    return (rules, my_ticket, tickets)


def test():
    rules, my_ticket, tickets = read_file("inputs/input16_test.txt")
    assert get_invalid_tickets(rules, tickets) == 71
    assert get_fields_order(rules, tickets) == ("row", "class", "seat")


def main():
    test()
    rules, my_ticket, tickets = read_file("inputs/input16.txt")
    print(get_invalid_tickets(rules, tickets))
    order = get_fields_order(rules, tickets)
    print(get_departure_values(my_ticket, order))


if __name__ == "__main__":
    main()
