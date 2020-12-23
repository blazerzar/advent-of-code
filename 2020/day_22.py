import os
from typing import Tuple, List


def winning_score(deck_a: List[int], deck_b: List[int]) -> int:
    """Return winning player's score"""
    while deck_a and deck_b:
        card_a = deck_a.pop(0)
        card_b = deck_b.pop(0)
        (deck_a if card_a > card_b else deck_b).extend(
            [card_a, card_b] if card_a > card_b else [card_b, card_a])
    return sum((i + 1) * n for i,
               n in enumerate((deck_a if deck_a else deck_b)[::-1]))


def recursive_winner(deck_a: List[int], deck_b: List[int]) -> int:
    """Return winning player's score in the recursive verions of the game"""
    seen_decks = set()  # Contains strings: str(deck_a) + str(deck_b)
    while deck_a and deck_b:
        decks_code = str(deck_a) + str(deck_b)
        # Player one is instant winner
        if decks_code in seen_decks:
            deck_b = []
            break
        seen_decks.add(decks_code)

        card_a = deck_a.pop(0)
        card_b = deck_b.pop(0)

        if len(deck_a) >= card_a and len(deck_b) >= card_b:
            # Both players can play another resursive game
            sub_result = recursive_winner(deck_a[:card_a], deck_b[:card_b])
            (deck_a if sub_result > 0 else deck_b).extend(
                [card_a, card_b] if sub_result > 0 else [card_b, card_a])
        else:
            # Normal round
            (deck_a if card_a > card_b else deck_b).extend(
                [card_a, card_b] if card_a > card_b else [card_b, card_a])

    # Negative results means player b won
    return (1 if deck_a else -1) * sum((i + 1) * n for i, n in enumerate(
        (deck_a if deck_a else deck_b)[::-1]))


def read_file(file_name: str) -> Tuple[List[int], List[int]]:
    return tuple([[int(line) for line in player.split("\n")[1:]]
                  for player in open(os.path.join(
                      os.path.dirname(__file__), file_name), "r")
                  .read().split("\n\n")])


def test():
    deck_a, deck_b = read_file("inputs/input22_test.txt")
    assert winning_score(deck_a[:], deck_b[:]) == 306
    assert abs(recursive_winner(deck_a[:], deck_b[:])) == 291


def main():
    test()
    deck_a, deck_b = read_file("inputs/input22.txt")
    print(winning_score(deck_a[:], deck_b[:]))
    print(abs(recursive_winner(deck_a[:], deck_b[:])))


if __name__ == "__main__":
    main()
