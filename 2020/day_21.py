import os
from typing import List, Tuple, Set, Dict
from collections import defaultdict


def num_without_allergens(food_list: List[Tuple[Set[str], Set[str]]]) \
        -> Tuple[int, Dict[str, Set[str]]]:
    """Return how many food from the list cannot contain any allergen"""
    allergens_possible = dict()
    num_of_appearances = defaultdict(int)

    for foods, allergens in food_list:
        # Increase appearances for each food
        for food in foods:
            num_of_appearances[food] += 1

        # Add foods to allergens set
        for allergen in allergens:
            if allergen not in allergens_possible.keys():
                allergens_possible[allergen] = set(foods)
            else:
                allergens_possible[allergen] &= foods

    food_with_allergens = set.union(*(set(x)
                                      for x in allergens_possible.values()))

    count = 0
    for food, appearances in num_of_appearances.items():
        count += appearances if food not in food_with_allergens else 0
    return count, allergens_possible


def find_allergens(possible: Dict[str, Set[str]]) -> str:
    """Return a string of allergens ordered alphabetically"""
    allergens = []
    while possible:
        to_be_deleted = []
        for allergen in possible:
            if len(possible[allergen]) == 1:
                allergens.append((allergen, possible[allergen].pop()))
                to_be_deleted.append(allergen)
                for allergen_tmp in possible:
                    if allergens[-1][1] in possible[allergen_tmp]:
                        possible[allergen_tmp].remove(allergens[-1][1])

        # Delete found allergens
        while to_be_deleted:
            del possible[to_be_deleted.pop()]

    result = ",".join(
        allergen[1] for allergen in sorted(
            allergens, key=lambda x: x[0]))
    return result


def read_file(file_name: str) -> List[Tuple[Set[str], Set[str]]]:
    return [(set(line.split("(")[0].strip().split(" ")),
             set(line.split("(")[1].strip()[9:-1].split(", ")))
            for line in open(os.path.join(
                os.path.dirname(__file__), file_name), "r")]


def test():
    food_list = read_file("inputs/input21_test.txt")
    count, possible = num_without_allergens(food_list)
    assert count == 5
    assert find_allergens(possible) == "mxmxvkd,sqjhc,fvjkl"


def main():
    test()
    food_list = read_file("inputs/input21.txt")
    count, possible = num_without_allergens(food_list)
    print(count)
    print(find_allergens(possible))


if __name__ == "__main__":
    main()
