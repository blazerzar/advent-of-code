import os
from typing import Set, Tuple


def simulate_cycles(cycles: int, active_input: Set[Tuple[int, int, int]],
                    d: int) -> int:
    """Simulate <cycles> cycles and return number of cubes in active state"""
    active = set(active_input)

    for _ in range(cycles):
        changed = set()

        # Check which active cubes will become inactive
        for ax, ay, az, aw in active:
            if not (2 <= active_neighbours(active, ax, ay, az, aw, d) <= 3):
                changed.add((ax, ay, az, aw))
            # Check which inactive cubes will become active
            for ix, iy, iz, iw in inactive_around_active(active, ax, ay,
                                                         az, aw, d):
                if (ix, iy, iz, iw) not in changed and active_neighbours(
                        active, ix, iy, iz, iw, d) == 3:
                    changed.add((ix, iy, iz, iw))

        # Update cubes
        active = active.symmetric_difference(changed)
    return len(active)


def active_neighbours(active: Set[Tuple[int, int, int]], x: int, y: int,
                      z: int, w: int, d: int) -> int:
    """Return number of active neighbours around the given cube"""
    count = 0
    for dx in range(-1, 2):
        for dy in range(-1, 2):
            for dz in range(-1, 2):
                for dw in ([0] if d == 3 else range(-1, 2)):
                    if dx != 0 or dy != 0 or dz != 0 or dw != 0:
                        count += (x + dx, y + dy, z + dz, w + dw) in active
    return count


def inactive_around_active(
        active: Set[Tuple[int, int, int]], x: int, y: int,
        z: int, w: int, d: int) -> Set[Tuple[int, int, int, int]]:
    """Return set of inactive cubes around the given cube"""
    inactive = set()
    for dx in range(-1, 2):
        for dy in range(-1, 2):
            for dz in range(-1, 2):
                for dw in ([0] if d == 3 else range(-1, 2)):
                    if ((dx != 0 or dy != 0 or dz != 0 or dw != 0) and
                            (x + dx, y + dy, z + dz, w + dw) not in active):
                        inactive.add((x + dx, y + dy, z + dz, w + dw))
    return inactive


def read_file(file_name: str) -> Set[Tuple[int, int, int]]:
    return {(x, y, 0, 0) for y, line in enumerate(open(os.path.join(
        os.path.dirname(__file__), file_name), "r"))
        for x, c in enumerate(line) if c == "#"}


def test():
    active = read_file("inputs/input17_test.txt")
    assert simulate_cycles(6, active, 3) == 112
    assert simulate_cycles(6, active, 4) == 848


def main():
    test()
    active = read_file("inputs/input17.txt")
    print(simulate_cycles(6, active, 3))
    print(simulate_cycles(6, active, 4))


if __name__ == "__main__":
    main()
