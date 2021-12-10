#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <stack>
#include <utility>

const int NEIGHBORS[4][2] = {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1}
};

std::vector<std::vector<int>> read_file(const std::string filename) {
    std::vector<std::vector<int>> result;
    
    std::ifstream in;
    in.open(filename);

    std::string line;
    while (std::getline(in, line)) {
        char c;
        std::istringstream ss(line);
        std::vector<int> nums_line;

        while (ss >> c) {
            nums_line.push_back(c - '0');
        }
        result.push_back(nums_line);
    } 

    return result;
}

/* Find low points and sum their risk levels */
int risk_levels_sum(const std::vector<std::vector<int>> &data) {
    int sum = 0;

    // For every cell ...
    for (decltype(data.size()) i = 0; i < data.size(); ++i) {
        for (decltype(data[i].size()) j = 0; j < data[i].size(); ++j) {
            bool lowest = true;
            // ... check every neighbor 
            for (int k = 0; k < 4 && lowest; ++k) {
                decltype(i) new_i = i + NEIGHBORS[k][0];
                decltype(j) new_j = j + NEIGHBORS[k][1];

                if (new_i >= 0 && new_i < data.size() && new_j >= 0 && new_j < data[new_i].size()) {
                    lowest = data[new_i][new_j] > data[i][j];  
                }
            }

            sum += lowest * (data[i][j] + 1);
        }
    }

    return sum;
}

/* Find size of the basin centered at row i and column j */
int basin_size(std::vector<std::vector<int>> data, int i, int j) {
    std::stack<std::pair<int, int>> points;
    points.push(std::make_pair(i, j));
    data[i][j] = -1;

    int size = 0;

    // Take one point from stack and add their higer neighbors
    while (!points.empty()) {
        auto point = points.top();
        ++size;
        points.pop();

        for (int k = 0; k < 4; ++k) {
            decltype(i) new_i = point.first + NEIGHBORS[k][0];
            decltype(j) new_j = point.second + NEIGHBORS[k][1];

            // Check if neighbor is higher but not 9 
            if (new_i >= 0 && new_i < data.size() &&
                    new_j >= 0 && new_j < data[new_i].size() &&
                    data[new_i][new_j] > data[point.first][point.second] &&
                    data[new_i][new_j] != 9) {
                points.push(std::make_pair(new_i, new_j));
                data[new_i][new_j] = -1;
            }
        }
    }

    return size;
}

/* Find three largest basins and return their size */
int largest_basins(const std::vector<std::vector<int>> &data) {
    int first = 0;
    int second = 0;
    int third = 0;

    // For every cell ...
    for (decltype(data.size()) i = 0; i < data.size(); ++i) {
        for (decltype(data[i].size()) j = 0; j < data[i].size(); ++j) {
            bool lowest = true;
            // ... check every neighbor 
            for (int k = 0; k < 4 && lowest; ++k) {
                decltype(i) new_i = i + NEIGHBORS[k][0];
                decltype(j) new_j = j + NEIGHBORS[k][1];

                if (new_i >= 0 && new_i < data.size() && new_j >= 0 && new_j < data[new_i].size()) {
                    lowest = data[new_i][new_j] > data[i][j];  
                }
            }

            // If we found a low points, calculate basin size
            if (lowest) {
                int size = basin_size(data, i, j);

                // Check if this basin is current top 3
                if (size > first) {
                    third = second;
                    second = first;
                    first = size;
                } else if (size > second) {
                    third = second;
                    second = size;
                } else if (size > third) {
                    third = size;
                }
            }
        }
    }

    return first * second * third;
}

void test() {
    std::vector<std::vector<int>> test_data = read_file("inputs/input9_test.txt");
    
    assert(risk_levels_sum(test_data) == 15);
    assert(basin_size(test_data, 4, 6) == 9);
    assert(largest_basins(test_data) == 1134);
}

int main() {
    test();
    
    std::vector<std::vector<int>> data = read_file("inputs/input9.txt");
    std::cout << risk_levels_sum(data) << std::endl;
    std::cout << largest_basins(data) << std::endl;
    
    return 0;
}
