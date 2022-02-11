#include <iostream>
#include <fstream>
#include <regex>
#include <cassert>
#include <algorithm>
#include <vector>
#include <optional>
#include <numeric>

struct Point {
    int x;
    int y;
    int z;
};

struct Step {
    bool on;
    Point min;
    Point max;

    long num_of_cubes() const {
        return (on ? 1l : -1l) * (max.x - min.x + 1) * (max.y - min.y + 1) * (max.z - min.z + 1);
    }
};

/* Return intersection if it exists */
std::optional<Step> operator&(const Step &lhs, const Step &rhs) {
    Point new_min = {
        std::max(lhs.min.x, rhs.min.x),
        std::max(lhs.min.y, rhs.min.y),
        std::max(lhs.min.z, rhs.min.z)
    };
    Point new_max = {
        std::min(lhs.max.x, rhs.max.x),
        std::min(lhs.max.y, rhs.max.y),
        std::min(lhs.max.z, rhs.max.z)
    };

    // No intersection
    if (new_min.x > new_max.x || new_min.y > new_max.y || new_min.z > new_max.z) {
        return {};
    }

    return Step{ !lhs.on, new_min, new_max };
}

/* Turn on or off cubes in the given range */
void execute_step(const Step step, bool cubes[101][101][101]) {
    for (int x = std::max(-50, step.min.x); x <= std::min(50, step.max.x); ++x) {
        for (int y = std::max(-50, step.min.y); y <= std::min(50, step.max.y); ++y) {
            for (int z = std::max(-50, step.min.z); z <= std::min(50, step.max.z); ++z) {
                cubes[x + 50][y + 50][z + 50] = step.on; 
            }
        }
    }
}

/* Return number of turned on cubes in a small space after executing reboot steps */
int reboot(std::vector<Step> steps) {
    bool cubes[101][101][101] = {};

    for (const Step step : steps) {
        execute_step(step, cubes);
    }
    
    // Count turned on cubes
    int on = 0;
    for (int x = 0; x < 101; ++x) {
        for (int y = 0; y < 101; ++y) {
            for (int z = 0; z < 101; ++z) {
                on += cubes[x][y][z];
            }
        }
    }

    return on;
}

/* Return number of turned on cubes in the whole space after executing reboot steps */
long turned_on_cubes(const std::vector<Step> &steps) {
    std::vector<Step> cubes;

    for (const Step step : steps) {
        std::vector<Step> intersections;
        // Check if [step] intersects with any cubes in [cubes]
        for (const Step cube : cubes) {
            auto intersect = cube & step;
            // Intersection exists
            if (intersect) {
                intersections.push_back(*intersect);
            }
        }
        
        // Add new intersections for compensation
        std::copy(intersections.begin(), intersections.end(), std::back_inserter(cubes));
        
        // Add positive instructions
        if (step.on) {
            cubes.push_back(step);
        }
    }

    return std::accumulate(cubes.begin(), cubes.end(), 0l, [](long a, Step b) {
        return a + b.num_of_cubes();
    });
}

std::vector<Step> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);
    std::vector<Step> steps;
    
    std::string line;
    std::regex step_regex("(on|off) x=(-?\\d+)..(-?\\d+),y=(-?\\d+)..(-?\\d+),z=(-?\\d+)..(-?\\d+)");
    std::smatch match;

    while (std::getline(in, line)) {
        std::regex_match(line, match, step_regex);
        steps.push_back({
            match.str(1) == "on", {
                std::stoi(match.str(2)),
                std::stoi(match.str(4)),
                std::stoi(match.str(6))
            }, {
                std::stoi(match.str(3)),
                std::stoi(match.str(5)),
                std::stoi(match.str(7))
            }
        });
    }

    return steps;
}

void test() {
    std::vector<Step> steps = read_file("inputs/input22_test1.txt");
    assert(reboot(steps) == 590784);
    steps = read_file("inputs/input22_test2.txt");
    assert(turned_on_cubes(steps) == 2758514936282235l);
}

int main() {
    test();

    std::vector<Step> steps = read_file("inputs/input22.txt");
    std::cout << reboot(steps) << '\n';
    std::cout << turned_on_cubes(steps) << '\n';
    
    return 0;
}

