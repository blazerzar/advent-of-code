import os
from typing import List, Callable


def can_remove_paratheses(expression: str) -> bool:
    """Return true if the outside parentheses can be removed"""
    level = 0
    for i, c in enumerate(expression):
        level += 1 if c == "(" else (-1 if c == ")" else 0)
        if level == 0 and i + 1 < len(expression):
            return False
    return level == 0


def next_operator(expression: str) -> int:
    """Return index of the next free operator"""
    level = 0
    for i, c in enumerate(expression[::-1]):
        level += 1 if c == "(" else (-1 if c == ")" else 0)
        if level == 0 and c in {"+", "*"}:
            return len(expression) - 1 - i
    return -1


def next_operator_part2(expression: str) -> int:
    """Return index of the next free operator
    if addition is evaluated before multiplication
    """
    level = 0
    last_plus = -1
    for i, c in enumerate(expression[::-1]):
        level += 1 if c == "(" else (-1 if c == ")" else 0)
        if level == 0:
            if c == "+":
                last_plus = i
            elif c == "*":
                return len(expression) - 1 - i
    return len(expression) - 1 - last_plus


def evaluate_expression(
        expression: str, operator_func: Callable[[str], int]) -> int:
    """Return value of the expression"""
    # Check if expression starts and ends with parentheses and can be removed
    if (expression[0] == "(" and expression[-1] == ")"
            and can_remove_paratheses(expression)):
        expression = expression[1:-1]

    # If expression is just a number, return it
    if expression.isnumeric():
        return int(expression)

    # Get index of the next operator to be evaluated
    operator = operator_func(expression)
    value = eval(str(evaluate_expression(expression[:operator], operator_func))
                 + expression[operator]
                 + str(evaluate_expression(expression[operator + 1:],
                                           operator_func)))
    return value


def read_file(file_name: str) -> List[str]:
    return [line.strip().replace(" ", "") for line in open(os.path.join(
        os.path.dirname(__file__), file_name), "r")]


def test():
    assert can_remove_paratheses("(5+5)*(2*6)") is False
    assert can_remove_paratheses(
        "(((12+2)*6+10)*(15+2)+(11+2)*(12*12)*2)") is True
    assert next_operator("(5+5)*(2*6)") == 5
    assert evaluate_expression("2*3+(4*5)", next_operator) == 26
    assert evaluate_expression("5+(8*3+9+3*4*3)", next_operator) == 437
    assert evaluate_expression(
        "5*9*(7*3*3+9*3+(8+6*4))", next_operator) == 12240
    assert evaluate_expression(
        "((2+4*9)*(6+9*8+6)+6)+2+4*2", next_operator) == 13632
    assert next_operator_part2("2*3+(4*5)") == 1
    assert evaluate_expression("1+(2*3)+(4*(5+6))", next_operator_part2) == 51
    assert evaluate_expression("2*3+(4*5)", next_operator_part2) == 46
    assert evaluate_expression("5+(8*3+9+3*4*3)", next_operator_part2) == 1445
    assert evaluate_expression(
        "5*9*(7*3*3+9*3+(8+6*4))", next_operator_part2) == 669060
    assert evaluate_expression(
        "((2+4*9)*(6+9*8+6)+6)+2+4*2", next_operator_part2) == 23340


def main():
    test()
    expressions = read_file("inputs/input18.txt")
    print(sum(evaluate_expression(expression, next_operator)
              for expression in expressions))
    print(sum(evaluate_expression(expression, next_operator_part2)
              for expression in expressions))


if __name__ == "__main__":
    main()
