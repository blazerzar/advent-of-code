#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <cassert>

struct Octopus {
    int y;
    int x;
};

const int NEIGHBORS[8][2] = {
    { -1, -1 }, { -1, 0 }, { -1, 1},
    { 0, -1 }, { 0, 1 },
    { 1, -1 }, { 1, 0 }, { 1, 1},
};

std::vector<std::vector<int>> read_file(const std::string &filename) {
    std::vector<std::vector<int>> grid;

    std::ifstream in;
    in.open(filename);

    std::string line_str;
    while (std::getline(in, line_str)) {
        std::vector<int> nums;
        std::istringstream line(line_str);
        
        char c;
        while (line >> c) {
            nums.push_back(c - '0');
        }

        grid.push_back(nums);
    }

    return grid;
}

/* Simulate one step of the iteration and return number of flashes */
int simulate_step(std::vector<std::vector<int>> &octopuses) {
    int flashes = 0;
    std::queue<Octopus> to_flash;

    // Increase energy level of each octopus
    for (int y = 0; y < octopuses.size(); ++y) {
        for (int x = 0; x < octopuses[y].size(); ++x) {
            int &energy = octopuses[y][x];

            // Only increase energy if smaller than 10
            if (energy < 10) {
                // Increase energy and check if octopus flashes
                if (++energy == 10) {
                    Octopus o = { y, x };
                    to_flash.push(o);
                    ++flashes;
                }
            }
        } 
    }

    // Flash octopuses
    while (!to_flash.empty()) {
        Octopus o = to_flash.front();
        to_flash.pop();

        // Increase energy of neighbors if they are not flashing
        for (int i = 0; i < 8; ++i) {
            int dy = NEIGHBORS[i][0];
            int dx = NEIGHBORS[i][1];

            int new_y = o.y + dy;
            int new_x = o.x + dx;

            if (new_y >= 0 && new_y < octopuses.size() &&
                    new_x >=0 && new_x < octopuses[new_y].size()) {
                int &energy = octopuses[new_y][new_x];

                if (energy < 10 && ++energy == 10) {
                    Octopus o = { new_y, new_x };
                    to_flash.push(o);
                    ++flashes;
                }
            }
        }
    }

    // Reset energy 10 to 0
    for (std::vector<int> &line : octopuses) {
        for (int &octopus : line) {
            octopus = octopus == 10 ? 0 : octopus;
        }
    }

    return flashes;
}

/* Calculate number of flashes in 100 steps */
int num_of_flashes(std::vector<std::vector<int>> octopuses) {
    int flashes = 0;

    // Simulate <steps> steps
    for (int step = 1; step <= 100; ++step) {
        flashes += simulate_step(octopuses);
    }
        
    return flashes;
} 

/* Find number of steps required for all octopuses to flash at once */
int flash_sync(std::vector<std::vector<int>> octopuses) {
    int steps = 1;
    int octopuses_num = octopuses.size() * octopuses[0].size();

    // Simulate steps until all octopuses flash
    while (simulate_step(octopuses) != octopuses_num) {
        ++steps;
    }

    return steps;
}

void test() {
    std::vector<std::vector<int>> test_data = read_file("inputs/input11_test.txt");

    assert(num_of_flashes(test_data) == 1656);
    assert(flash_sync(test_data) == 195);
}

int main() {
    test();

    std::vector<std::vector<int>> data = read_file("inputs/input11.txt");
    std::cout << num_of_flashes(data) << std::endl;
    std::cout << flash_sync(data) << std::endl;
    
    return 0;
}

