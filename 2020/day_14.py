import os
from typing import List, Tuple


def execute_instructions(
        instructions: List[Tuple[str, str]], version: int) -> int:
    """Execute given instructions and return sum of values left in memory"""
    memory = dict()
    mask = "X" * 36
    for instr, value in instructions:
        if instr == "mask":
            mask = value
        elif version == 1:
            address = int(instr.replace("mem[", "").replace("]", ""))
            memory[address] = use_mask(mask, int(value))
        elif version == 2:
            address = int(instr.replace("mem[", "").replace("]", ""))
            addresses = use_mask_version2(mask, address)
            for addr in addresses:
                memory[addr] = int(value)
    return sum(memory.values())


def use_mask(mask: str, number: int) -> int:
    """Return number masked with the given mask"""
    mask_ones = int(mask.replace("X", "0"), 2)
    mask_zeroes = int(mask.replace("X", "1"), 2)
    return (number | mask_ones) & mask_zeroes


def use_mask_version2(mask: str, number: int) -> Tuple[int, ...]:
    """Return multiple addresses that are produced using the given mask"""
    mask_ones = int(mask.replace("X", "0"), 2)
    mask_zeroes = int(mask.replace("0", "1").replace("X", "0"), 2)
    addresses = [(number | mask_ones) & mask_zeroes]
    for i in [i for i, c in enumerate(mask[::-1]) if c == "X"]:
        new_addresses = []
        for a in addresses:
            new_addresses.append(a + 2**i)
        addresses += new_addresses
    return tuple(addresses)


def read_file(file_name: str) -> List[Tuple[str, str]]:
    return [tuple(line.strip().split(" = ")) for line in open(os.path.join(
        os.path.dirname(__file__), file_name), "r")]


def test():
    instructions = read_file("inputs/input14_test1.txt")
    assert use_mask("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X", 11) == 73
    assert use_mask("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X", 101) == 101
    assert use_mask("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X", 0) == 64
    assert execute_instructions(instructions, 1) == 165
    assert use_mask_version2(
        "000000000000000000000000000000X1001X", 42) == (26, 27, 58, 59)
    assert use_mask_version2(
        "00000000000000000000000000000000X0XX", 26) == (
        16, 17, 18, 19, 24, 25, 26, 27)
    instructions = read_file("inputs/input14_test2.txt")
    assert execute_instructions(instructions, 2) == 208


def main():
    test()
    instructions = read_file("inputs/input14.txt")
    print(execute_instructions(instructions, 1))
    print(execute_instructions(instructions, 2))


if __name__ == "__main__":
    main()
