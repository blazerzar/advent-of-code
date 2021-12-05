#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <regex>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <new>

struct Line {
    int x1;
    int y1;
    int x2;
    int y2;
};

/* Read and parse input data in vector of Lines */
std::pair<std::vector<Line>, int *> read_file(const std::string &filename) {
    std::vector<Line> lines;

    std::ifstream in;
    in.open(filename);
    std::string line_str;

    // Create regex for parsing data
    std::regex regex("^(\\d+),(\\d+) -> (\\d+),(\\d+)$");

    // Keep track of the border coordinates
    int x_min = 0, y_min = 0, x_max = 0, y_max = 0;

    while (std::getline(in, line_str)) {
        std::smatch match;
        if (std::regex_match(line_str, match, regex)) {
            Line line;
            line.x1 = std::stoi(match[1]);
            line.y1 = std::stoi(match[2]);
            line.x2 = std::stoi(match[3]);
            line.y2 = std::stoi(match[4]);
            lines.push_back(line);

            if (line.x1 < x_min) x_min = line.x1;
            if (line.x2 < x_min) x_min = line.x2;
            if (line.x1 > x_max) x_max = line.x1;
            if (line.x1 > x_max) x_max = line.x2;

            if (line.y1 < y_min) y_min = line.y1;
            if (line.y2 < y_min) y_min = line.y2;
            if (line.y1 > y_max) y_max = line.y1;
            if (line.y2 > y_max) y_max = line.y2;
        }

    } 

    int *limits = new int[4];
    limits[0] = x_min; limits[1] = y_min; limits[2] = x_max; limits[3] = y_max;
    return std::make_pair(lines, limits);
}

/* Find points where at least 2 vertical or horizontal lines intersect */
int find_danger_points(const std::vector<Line> &lines, int height, int width) {
    int diagram[height][width] = {0}; 
    std::memset(diagram, 0, sizeof(diagram));
    int danger_points = 0;
    
    for (const Line line : lines) {
        // Only consider vertical and horizontal lines
        if (line.x1 == line.x2) {            
            for (int y = std::min(line.y1, line.y2); y <= std::max(line.y1, line.y2); ++y) {
                ++diagram[y][line.x1];
                if (diagram[y][line.x1] == 2) ++danger_points;
            }
        } 

        if (line.y1 == line.y2) {
            for (int x = std::min(line.x1, line.x2); x <= std::max(line.x1, line.x2); ++x) {
                ++diagram[line.y1][x];
                if (diagram[line.y1][x] == 2) ++danger_points;
            }
        }
   }
    
    return danger_points;
}

/* Find points where at least 2 lines intersect */
int find_danger_points_diag(const std::vector<Line> &lines, int height, int width) {
    int diagram[height][width] = {0}; 
    std::memset(diagram, 0, sizeof(diagram));
    int danger_points = 0;

    for (const Line line : lines) {
        int dx = line.x1 == line.x2 ? 0 : (line.x2 - line.x1) / (std::max(line.x1, line.x2) - std::min(line.x1, line.x2));
        int dy = line.y1 == line.y2 ? 0 : (line.y2 - line.y1) / (std::max(line.y1, line.y2) -  std::min(line.y1, line.y2));

        for (int x = line.x1, y = line.y1; ; x += dx, y += dy) {
            ++diagram[y][x];
            if (diagram[y][x] == 2) ++danger_points;

            if (x == line.x2 && y == line.y2) break;
        }
   }
    
    return danger_points;
}

void test() {
    auto file_data = read_file("inputs/input5_test.txt");
    std::vector<Line> test_data = file_data.first;
    int *limits = file_data.second;

    assert(find_danger_points(test_data, limits[3] - limits[1] + 1, limits[2] - limits[0] + 1) == 5); 
    assert(find_danger_points_diag(test_data, limits[3] - limits[1] + 1, limits[2] - limits[0] + 1) == 12); 
    
    delete[] limits;
}

int main() {
    test();

    auto file_data = read_file("inputs/input5.txt");
    std::vector<Line> data = file_data.first;
    int *limits = file_data.second;
    std::cout << find_danger_points(data, limits[3] - limits[1] + 1, limits[2] - limits[0] + 1) << std::endl; 
    std::cout << find_danger_points_diag(data, limits[3] - limits[1] + 1, limits[2] - limits[0] + 1) << std::endl; 

    delete[] limits;

    return 0;
}
