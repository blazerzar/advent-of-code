from typing import List


def simulate_moves(cups_in: List[int], n: int) -> List[int]:
    """Return cups ordered after <n> moves"""
    cups = [0] * (len(cups_in) + 1)
    for i, cup in enumerate(cups_in):
        cups[cup] = cups_in[(i + 1) % len(cups_in)]

    current_cup = cups_in[0]

    for _ in range(n):
        # Get the next 3 cups
        a = cups[current_cup]
        b = cups[a]
        c = cups[b]

        # Remove them
        cups[current_cup] = cups[c]

        # Find destination cup
        destination_cup = (current_cup - 2) % len(cups_in) + 1
        while destination_cup in {a, b, c}:
            destination_cup = (destination_cup - 2) % len(cups_in) + 1

        # Return the 3 removed cups back
        cups[c] = cups[destination_cup]
        cups[destination_cup] = a

        # Change the current cup
        current_cup = cups[current_cup]

    return cups


def get_result(input_pos: str, n: int) -> str:
    cups_in = [int(cup) for cup in input_pos]
    cups = simulate_moves(cups_in, n)
    output_current = 1
    output = []
    for _ in range(len(cups_in)):
        output.append(str(output_current))
        output_current = cups[output_current]
    return "".join(output[1:])


def find_stars(input_pos: str, n: int) -> int:
    """Return product of cups that hide the stars"""
    cups = [int(cup) for cup in input_pos] + \
        [cup for cup in range(10, 1_000_000 + 1)]
    cups = simulate_moves(cups, n)
    return cups[1] * cups[cups[1]]


def test():
    assert get_result("389125467", 10) == "92658374"
    assert get_result("389125467", 100) == "67384529"
    assert find_stars("389125467", 10_000_000) == 149245887792


def main():
    test()
    print(get_result("167248359", 100))
    print(find_stars("167248359", 10_000_000))


if __name__ == "__main__":
    main()
