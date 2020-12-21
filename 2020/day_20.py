import os
from math import sqrt
from typing import List, Set


class Tile:
    """Holds informatin about each edge of the tile"""

    def __init__(self, id: int, top: int, bottom: int,
                 left: int, right: int, image: List[str]):
        self.id = id
        self.top = top
        self.bottom = bottom
        self.left = left
        self.right = right
        self.image = image

    def __repr__(self):
        return str(self.id)

    def print_image(self):
        for line in self.image:
            print(line)
        print()


def create_flipped(tile: Tile) -> Tile:
    """Return instance of class Tile that represent a flipped version
    of the given tile
    """
    return Tile(tile.id, tile.bottom, tile.top,
                int(format(tile.left, "010b")[::-1], 2),
                int(format(tile.right, "010b")[::-1], 2), tile.image[::-1])


def create_all_symmetries(tile: Tile) -> List[Tile]:
    top, bottom, left, right, image = (tile.top, tile.bottom, tile.left,
                                       tile.right, tile.image)
    tiles = []

    # Add all rotated tiles
    for _ in range(4):
        tiles.append(Tile(tile.id, top, bottom, left, right, image))
        top, bottom, left, right, image = (int(format(left, "010b")[::-1], 2),
                                           int(format(right, "010b")[::-1], 2),
                                           bottom,
                                           top,
                                           ["".join(line) for line in
                                            list(zip(*reversed(image)))])

    # Add all flipped tiles
    for t in tiles[:]:
        tiles.append(create_flipped(t))
    return tiles


def arrange_rec(tiles: List[Tile], square: List[List[Tile]],
                i: int, j: int, used: Set[int]) -> List[List[Tile]]:
    """Recursive function for making arrangements"""
    # If we came to the end, we have found an arrangement
    if i == len(square):
        return square

    # Try to add each tile to the current position
    for tile in [t for t in tiles if t.id not in used]:
        # Check all possible orientaions to see if it fits
        for t in create_all_symmetries(tile):
            if ((i == 0 or square[i - 1][j].bottom == t.top)
                    and (j == 0 or square[i][j - 1].right == t.left)):
                square[i][j] = t
                used.add(t.id)
                result = arrange_rec(tiles, square,
                                     i if j + 1 < len(square) else i + 1,
                                     j + 1 if j + 1 < len(square) else 0, used)
                if result:
                    return result
                used.remove(t.id)
    return None


def arrange_tiles(tiles: List[Tile]) -> List[List[Tile]]:
    """Create an arrangement of given tiles into a square"""
    size = int(sqrt(len(tiles)))
    return arrange_rec(
        tiles, [[None] * size for _ in range(size)], 0, 0, set())


def build_image(square: List[List[Tile]]) -> List[str]:
    size = len(square) * len(square[0][0].image)
    image = []
    for i in range(size):
        image.append("")
        for t in square[i // (size // len(square))]:
            image[i] += t.image[i % len(t.image)]
    return image


def rotate_image(image: List[str]) -> List[str]:
    return ["".join(line) for line in list(zip(*reversed(image)))]


def flip_image(image: List[str]) -> List[str]:
    return image[::-1]


def find_sea_monsters(image: List[str]):
    """Edit image by replacing '#' that represent a sea monster with 'O'"""
    monster = [
        "                  # ",
        "#    ##    ##    ###",
        " #  #  #  #  #  #   "
    ]

    monster_coords = [(dy, dx) for dy, line in enumerate(monster)
                      for dx, c in enumerate(line) if c == "#"]

    for i in range(len(image) - len(monster)):
        for j in range(len(image[i]) - len(monster[0])):
            for dy, dx in monster_coords:
                if image[i + dy][j + dx] != "#":
                    break
            else:
                # We found a sea monster
                for dy, dx in monster_coords:
                    image[i + dy] = image[i + dy][:j + dx] + \
                        "O" + image[i + dy][j + dx + 1:]


def find_sea_monsters_all(image: List[str]):
    # Check all image symmetries for monsters
    for _ in range(4):
        # Check image
        find_sea_monsters(image)
        # Check flipped image
        image = flip_image(image)
        find_sea_monsters(image)
        # Rotate image
        image = rotate_image(image)
    return image


def read_file(file_name: str) -> List[Tile]:
    tiles = []
    for tile in open(os.path.join(os.path.dirname(__file__),
                                  file_name), "r").read().split("\n\n"):
        lines = tile.split("\n")
        tiles.append(
            Tile(int(lines[0][5:-1]),
                 int(lines[1].replace(".", "0").replace("#", "1"), 2),
                 int(lines[-1].replace(".", "0").replace("#", "1"), 2),
                 int("".join(line[0].replace(".", "0").replace("#", "1")
                             for line in lines[1:]), 2),
                 int("".join(line[-1].replace(".", "0").replace("#", "1")
                             for line in lines[1:]), 2),
                 [line[1:-1] for line in lines[2:-1]]))
    return tiles


def test():
    tiles = read_file("inputs/input20_test.txt")
    square = arrange_tiles(tiles)
    assert (square[0][0].id * square[0][-1].id * square[-1]
            [0].id * square[-1][-1].id == 20899048083289)
    image = build_image(square)

    find_sea_monsters_all(image)
    count = 0
    for line in image:
        count += line.count("#")
    assert count == 273


def main():
    test()
    tiles = read_file("inputs/input20.txt")
    square = arrange_tiles(tiles)
    print(square[0][0].id * square[0][-1].id * square[-1]
          [0].id * square[-1][-1].id)
    image = build_image(square)

    image = find_sea_monsters_all(image)
    count = 0
    for line in image:
        count += line.count("#")
    print(count)


if __name__ == "__main__":
    main()
