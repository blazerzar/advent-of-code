#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <cassert>
#include <cmath>

struct Area {
    int minX;
    int maxX;
    int minY;
    int maxY;

    bool hit(int x, int y) const {
        return x >= minX && x <= maxX && y >= minY && y <= maxY; 
    }
};

Area read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    std::string content;
    std::getline(in, content);

    std::regex content_regex("target area: x=(-?\\d+)..(-?\\d+), y=(-?\\d+)..(-?\\d+)");
    std::smatch match;
    
    std::regex_match(content, match, content_regex);
    return {
        std::stoi(match.str(1)),
        std::stoi(match.str(2)),
        std::stoi(match.str(3)),
        std::stoi(match.str(4))
    };
}

/* Find optimal velocities and return highest y achieved */
int highest_y(const Area target) {
    return target.minY * (target.minY + 1) / 2;
}

/* Return number of velocities using which we hit target */
int num_of_velocities(const Area target) {
    // Calculate min and max x velocity
    int min_vx = (-1 + std::sqrt(1 + 8 * target.minX)) / 2;
    int max_vx = target.maxX;

    // Calculate min and max y velocity
    int min_vy = target.minY;
    int max_vy = -target.minY - 1;
    
    int velocities = 0;

    for (int vx_0 = min_vx; vx_0 <= max_vx; ++vx_0) {
        for (int vy_0 = min_vy; vy_0 <= max_vy; ++vy_0) {
            int x = 0, y = 0;
            int vx = vx_0, vy = vy_0;

            while (x <= target.maxX && y >= target.minY) {
                if (vx > 0) {
                    x += vx--; 
                }
                y += vy--;

                // Check if target is hit
                if (target.hit(x, y)) {
                    ++velocities;
                    break;
                }
            }
        }
    }
    
    return velocities;
}

void test() {
    Area area = read_file("inputs/input17_test.txt");

    assert(highest_y(area) == 45);
    assert(num_of_velocities(area) == 112);
}

int main() {
    test();
    
    Area area = read_file("inputs/input17.txt");
    std::cout << highest_y(area) << '\n';
    std::cout << num_of_velocities(area) << '\n';

    return 0;
}
