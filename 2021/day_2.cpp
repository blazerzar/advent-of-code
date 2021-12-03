#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <cassert>

const int FORWARD = 0;
const int UP = 1;
const int DOWN = 2;

/* Parse input into vector of int pairs */
std::vector<std::pair<int, int>> read_file(const std::string &filename) {
    std::vector<std::pair<int, int>> input;

    std::ifstream in;
    in.open(filename);
    
    std::string direction;
    int value;
    while (in >> direction >> value) {
        if (direction == "forward") {
            input.push_back({FORWARD, value});
        } else if (direction == "up") {
            input.push_back({UP, value});
        } else if (direction == "down") {
            input.push_back({DOWN, value});
        } else {
            input.push_back({-1, -1});
        }
    }

    return input;
}

/* Return final horizontal and vertical position multiplied */
int find_position(const std::vector<std::pair<int, int>> instructions) {
    int x = 0, y = 0;

    for (auto instruction : instructions) {
        if (instruction.first == FORWARD) {
            x += instruction.second;
        } else if (instruction.first == UP) {
            y -= instruction.second;
        } else if (instruction.first == DOWN) {
            y += instruction.second;
        }
    }

    return x * y;
}

/* Return final horizontal and vertical position multiplied using aim */
int find_position_and_aim(const std::vector<std::pair<int, int>> instructions) {
    int x = 0, y = 0, aim = 0;

    for (auto instruction : instructions) {
        if (instruction.first == FORWARD) {
            x += instruction.second;
            y += aim * instruction.second;
        } else if (instruction.first == UP) {
            aim -= instruction.second;
        } else if (instruction.first == DOWN) {
            aim += instruction.second;
        }
    }

    return x * y;
}

void test() {
    std::vector<std::pair<int, int>> input = read_file("inputs/input2_test.txt");
    assert(find_position(input) == 150);
    assert(find_position_and_aim(input) == 900);
}

int main() {
    test(); 
    
    std::vector<std::pair<int, int>> input = read_file("inputs/input2.txt");
    std::cout << find_position(input) << std::endl;
    std::cout << find_position_and_aim(input) << std::endl;

    return 0;
}
