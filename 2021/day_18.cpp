#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <memory>
#include <vector>
#include <string>

struct SnailfishNumber {
    std::shared_ptr<SnailfishNumber> left;
    std::shared_ptr<SnailfishNumber> right;
    bool is_num;
    int number;

    SnailfishNumber(
            std::shared_ptr<SnailfishNumber> l,
            std::shared_ptr<SnailfishNumber> r,
            bool is_n,
            int n) :
        left(l), right(r), is_num(is_n), number(n) { }

    /* Copy the whole hierarchy of the number */
    SnailfishNumber(const SnailfishNumber &num) {
        if (num.left && num.right) {
            left = std::make_shared<SnailfishNumber>(SnailfishNumber(*num.left));
            right = std::make_shared<SnailfishNumber>(SnailfishNumber(*num.right));
        }
        is_num = num.is_num;
        number = num.number;
    }

    /* Try to explode this number */
    int explode(int depth = 0,
                std::shared_ptr<SnailfishNumber> first_left = nullptr,
                std::shared_ptr<SnailfishNumber> first_right = nullptr) {
        if (is_num || (depth == 4 && (!left->is_num || !right->is_num))) {
            // We do not explode numbers or pairs without regular numbers
            return 0;
        } else if (depth == 4) {
            // Find first numbers on the left ...
            if (first_left) {
                auto current = first_left;
                while (current->right) {
                    current = current->right;
                }
                // Add left element of exploded pair
                current->number += left->number;
            }

            // ... and right
            if (first_right) {
                auto current = first_right;
                while (current->left) {
                    current = current->left;
                }
                // Add right element of exploded pair
                current->number += right->number;
            }

            // 1 means we need to fix parent
            return 1;
        } else {
            // Try to explode children
            int result = left->explode(depth + 1, first_left, right);
            if (result == 1) {
                // Replace exploded pair with 0
                SnailfishNumber new_child = SnailfishNumber(nullptr, nullptr, true, 0);
                left = std::make_shared<SnailfishNumber>(new_child);
                // 2 means we stop the recursion
                return 2;
            } else if (result == 2) {
                return 2;
            }

            result = right->explode(depth + 1, left, first_right);
            if (result == 1) {
                // Replace exploded pair with 0
                SnailfishNumber new_child = SnailfishNumber(nullptr, nullptr, true, 0);
                right = std::make_shared<SnailfishNumber>(new_child);
                // 2 means we stop the recursion
                return 2;
            } else if (result == 2) {
                return 2;
            }

            // No explosion has been done
            return 0;
        }
    }

    /* Try to split this number */
    int split() {
        if (is_num) {
            // We split number if it is bigger than 10
            return number >= 10;
        } else {
            // We try to split left ...
            int result = left->split();
            if (result == 1) {
                // Split number >= 10 after it has been found
                SnailfishNumber new_left = SnailfishNumber(nullptr, nullptr, true, left->number / 2);
                SnailfishNumber new_right = SnailfishNumber(nullptr, nullptr, true,  left->number - new_left.number);
                SnailfishNumber new_child = SnailfishNumber(
                    std::make_shared<SnailfishNumber>(new_left),
                    std::make_shared<SnailfishNumber>(new_right),
                    false,
                    -1
                );
                left = std::make_shared<SnailfishNumber>(new_child);
                // 2 means we stop recursion
                return 2;
            } else if (result == 2) {
                return 2;
            }

            // ... and right child
            result = right->split();
            if (result == 1) {
                // Split number >= 10 after it has been found
                SnailfishNumber new_left = SnailfishNumber(nullptr, nullptr, true, right->number / 2);
                SnailfishNumber new_right = SnailfishNumber(nullptr, nullptr, true,  right->number - new_left.number);
                SnailfishNumber new_child = SnailfishNumber(
                    std::make_shared<SnailfishNumber>(new_left),
                    std::make_shared<SnailfishNumber>(new_right),
                    false,
                    -1
                );
                right = std::make_shared<SnailfishNumber>(new_child);
                // 2 means we stop recursion
                return 2;
            } else if (result == 2) {
                return 2;
            }

            // No split has been done
            return 0;
        }
    }

    /* Get magnitude of this number */
    int mag() {
        if (is_num) {
            // Magnitude of regular number is the number itself
            return number;
        }
        // Magnitude of pair is combined from magnitudes from left and right
        return 3 * left->mag() + 2 * right->mag();
    }

    /* Reduce this number */
    void reduce() {
        int result = 1;
        while (result != 0) {
            // Try to explode number
            result = explode();

            // Only split if number did not explode
            if (result == 0) {
                result = split();
            }
        }
    }
};

std::ostream &operator<<(std::ostream &os, const SnailfishNumber &num) {
    if (num.is_num) {
        os << num.number;
    } else {
        os << '[' << *num.left << ',' << *num.right << ']';
    }
    return os;
}

/* Add 2 numbers together and reduce the result */
SnailfishNumber operator+(const SnailfishNumber lhs, const SnailfishNumber rhs) {
    SnailfishNumber result = SnailfishNumber(
        std::make_shared<SnailfishNumber>(lhs),
        std::make_shared<SnailfishNumber>(rhs),
        false,
        -1
    );
    result.reduce();
    return result;
}

/* Parse string to snailfish number */
SnailfishNumber parse_number(const std::string &number) {
    // Find comma of the outmost pair
    int depth = 0;
    int comma = -1;
    for (decltype(number.size()) i = 1; i < number.size() && comma < 0; ++i) {
        char c = number[i];
        switch (c) {
            case '[':
                ++depth;
                break;
            case ']':
                --depth;
                break;
            case ',':
                if (depth == 0) {
                    comma = i;
                }
        }
    }

    // If comma is not found, this is a regular number
    if (comma == -1) {
        return SnailfishNumber(nullptr, nullptr, true, std::stoi(number));
    }

    // Split string into 2 numbers and parse them
    std::string left = number.substr(1, comma - 1);
    std::string right = number.substr(comma + 1, number.size() - comma - 2);

    auto left_num = std::make_shared<SnailfishNumber>(parse_number(left));
    auto right_num = std::make_shared<SnailfishNumber>(parse_number(right));

    return SnailfishNumber(left_num, right_num, false, -1);
}

/* Read file and parse every line as a number */
std::vector<SnailfishNumber> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);
    std::vector<SnailfishNumber> numbers;

    std::string line;
    while (std::getline(in, line)) {
        numbers.push_back(parse_number(line));
    }

    return numbers;
}

/* Calculate sum of all numbers */
SnailfishNumber add_numbers(const std::vector<SnailfishNumber> numbers) {
    SnailfishNumber result = numbers[0];
    for (decltype(numbers.size()) i = 1; i < numbers.size(); ++i) {
        result = result + numbers[i];
    }
    return result;
}

/* Find largest magnitude that is result of adding 2 numbers */
int largest_magnitude(const std::vector<SnailfishNumber> numbers) {
    int largest = 0;
    for (decltype(numbers.size()) i = 0; i < numbers.size(); ++i) {
        for (decltype(numbers.size()) j = 0; j < numbers.size(); ++j) {
            if (i != j) {
                int res = (numbers[i] + numbers[j]).mag();
                largest = res > largest ? res : largest;
            }
        }
    }
    return largest;
}

void test() {
    std::vector<SnailfishNumber> numbers = read_file("inputs/input18_test.txt");
    std::stringstream result;

    // Test explosion
    SnailfishNumber num = parse_number("[[[[[9,8],1],2],3],4]");
    num.explode();
    result << num;
    assert(result.str() == "[[[[0,9],2],3],4]");
    result.str("");

    num = parse_number("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]");
    num.explode();
    result << num;
    assert(result.str() == "[[3,[2,[8,0]]],[9,[5,[7,0]]]]");
    result.str("");

    // Test splitting
    num = parse_number("[[[[0,7],4],[15,[0,13]]],[1,1]]");
    num.split();
    result << num;
    assert(result.str() == "[[[[0,7],4],[[7,8],[0,13]]],[1,1]]");
    result.str("");

    num = parse_number("[[[[0,7],4],[[7,8],[0,13]]],[1,1]]");
    num.split();
    result << num;
    assert(result.str() == "[[[[0,7],4],[[7,8],[0,[6,7]]]],[1,1]]");
    result.str("");

    // Test addition
    num = parse_number("[[[[4,3],4],4],[7,[[8,4],9]]]") + parse_number("[1,1]");
    result << num;
    assert(result.str() == "[[[[0,7],4],[[7,8],[6,0]]],[8,1]]");
    result.str("");

    // Test magnitude
    num = parse_number("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]");
    assert(num.mag() == 3488);

    // Test part 1 and part 2
    assert(add_numbers(numbers).mag() == 4140);
    assert(largest_magnitude(numbers) == 3993);
}

int main() {
    test();

    std::vector<SnailfishNumber> numbers = read_file("inputs/input18.txt");
    std::cout << add_numbers(numbers).mag() << '\n';
    std::cout << largest_magnitude(numbers) << '\n';

    return 0;
}

