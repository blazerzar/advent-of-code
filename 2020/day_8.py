import os
from typing import List, Tuple


class Device:
    """Class that represents a handheld device with a specific program"""
    def __init__(self, program: List[Tuple[str, int]]):
        """Save the program and initialiaze acc and pc to 0"""
        self.program = program
        self.acc = 0
        self.pc = 0
        self.used = set()
        self.cycle = False

    def run(self):
        """Run the program until it goes into a cycle or device halts"""
        while (self.pc < len(self.program) and self.pc not in self.used):
            self.used.add(self.pc)
            if self.program[self.pc][0] == "nop":
                self.nop()
            elif self.program[self.pc][0] == "acc":
                self.acc_f(self.program[self.pc][1])
            elif self.program[self.pc][0] == "jmp":
                self.jmp(self.program[self.pc][1])
        if (self.pc in self.used):
            self.cycle = True

    def nop(self):
        """No operation, go to next instruction"""
        self.pc += 1

    def acc_f(self, arg: int):
        """Change accumulator by arg and go to next instruction"""
        self.acc += arg
        self.pc += 1

    def jmp(self, arg: int):
        """Change pc by arg"""
        self.pc += arg


def acc_value(program: List[Tuple[str, int]]) -> int:
    """Return value in acc after device halts"""
    device = Device(program)
    device.run()
    return device.acc


def fix_program(program: List[Tuple[str, int]]) -> int:
    """
    Fix the program by changing one nop or jmp instruction to the other one
    and return value in acc after device halts
    """
    for i, (instruction, arg) in enumerate(program):
        # Change jmp or nop instruction and check if device halts
        new_program = None
        if (instruction == "nop"):
            new_program = program[:]
            new_program[i] = ("jmp", arg)
        elif (instruction == "jmp"):
            new_program = program[:]
            new_program[i] = ("nop", arg)
        if (instruction == "nop" or instruction == "jmp"):
            device = Device(new_program)
            device.run()
            if not device.cycle:
                return device.acc


def test():
    program = read_file("inputs/input8_test.txt")
    assert acc_value(program) == 5
    assert fix_program(program) == 8


def read_file(file_name: str) -> List[Tuple[str, int]]:
    return [(ins, int(arg)) for ins, arg in [tuple(line.strip().split())
            for line in open(os.path.join(
                os.path.dirname(__file__), file_name))]]


def main():
    test()
    program = read_file("inputs/input8.txt")
    print(acc_value(program))
    print(fix_program(program))


if __name__ == "__main__":
    main()
