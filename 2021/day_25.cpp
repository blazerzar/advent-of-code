#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

/* Move all cucumbers in the selected herd */
void move_cucumbers(const std::vector<std::string> &cucumbers,
                    std::vector<std::string> &moved, char herd) {
    for (int y = 0; y < cucumbers.size(); ++y) {
        for (int x = 0; x < cucumbers[y].size(); ++x) {
            if (cucumbers[y][x] == herd) {
                if (herd == '>' && cucumbers[y][(x + 1) % cucumbers[y].size()] == '.') {
                    moved[y][x] = '.';
                    moved[y][(x + 1) % moved[y].size()] = herd;
                } else if (herd == 'v' && cucumbers[(y + 1) % cucumbers.size()][x] == '.') {
                    moved[y][x] = '.';
                    moved[(y + 1) % moved.size()][x] = herd;
                }
            }
        }
    }
}

/* Return step on which cucumbers stop moving */
int cucumbers_stop(std::vector<std::string> cucumbers) {
    for (int step = 1; true; ++step) {
        std::vector<std::string> moved = cucumbers;

        // Move cucumbers facing east
        move_cucumbers(cucumbers, moved, '>'); 
        bool moved_east = cucumbers != moved;
        cucumbers = moved;

        // Move cucumbers facing south
        move_cucumbers(cucumbers, moved, 'v'); 

        // Check if movement stopped
        if (!moved_east && moved == cucumbers) {
            return step;
        }

        cucumbers = std::move(moved);
    }

    return -1;
}

std::vector<std::string> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);
    
    std::vector<std::string> cucumbers;
    std::string line;
    while (std::getline(in, line)) {
        cucumbers.push_back(line);
    }

    return cucumbers;
}

void test() {
    std::vector<std::string> cucumbers = read_file("inputs/input25_test.txt");
    assert(cucumbers_stop(cucumbers) == 58);
}

int main() {
    test();

    std::vector<std::string> cucumbers = read_file("inputs/input25.txt");
    std::cout << cucumbers_stop(cucumbers) << '\n';
    
    return 0;
}

