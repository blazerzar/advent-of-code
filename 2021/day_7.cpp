#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cassert>

std::vector<int> read_file(const std::string &filename) {
    std::vector<int> input;

    std::ifstream in; 
    in.open(filename);
    int num;
    char temp;

    while (in >> num) {
        input.push_back(num);

        in >> temp;
    }

    return input;
}

/* Calculate optimum horizontal positions to minimize moves */
int align_crabs(std::vector<int> positions) {
    std::sort(positions.begin(), positions.end());

    int left = 0;
    int right = std::accumulate(positions.cbegin(), positions.cend(), 0, std::plus<int>());
    right -= (positions.size() - 1) * left;

    int optimum = positions[0];
    int fuel = left + right;
    
    decltype(positions.size()) i = 0;

    for (int pos = 1; pos <= positions[positions.size() - 1]; ++pos) {
        // Find number of crabs at pos-1 (how much crabs move to the other size)
        int change = 0;
        while (i < positions.size() && positions[i] == pos - 1) {
            ++i;
            ++change;
        }

        // Distance of crabs to the left increases
        left += i + change;
        // Distance of crabs to the right decreases
        right -= positions.size() - i + change;

        if (left + right < fuel) {
            optimum = i;
            fuel = left + right;
        }
    }

    return fuel;
}

int align_crabs_linear(const std::vector<int> &positions) {
    // Create a vector where value is number of crabs at position
    int min_value = *std::min_element(positions.cbegin(), positions.cend());
    int max_value = *std::max_element(positions.cbegin(), positions.cend());
    std::vector<int> positions_vec(max_value - min_value + 1);
    for (const int pos : positions) {
        ++positions_vec[pos - min_value];
    }

    // Calculate initial price
    int left = 0;
    int right = 0; 
    for (decltype(positions_vec.size()) i = 1; i < positions_vec.size(); ++i) {
        right += positions_vec[i] * i * (i + 1) / 2;
    }

    int optimum = 0;
    int fuel = left + right;

    // Check every possible position
    for (int current_pos = 1; current_pos < positions_vec.size(); ++current_pos) {
        // Increase price to the left for every crab on that side
        for (int j = 0; j < current_pos; ++j) {
            left += positions_vec[j] * (current_pos - j);
        }

        // Decrease price to the right for every crab on that side
        for (int j = current_pos; j < positions_vec.size(); ++j) {
            right -= positions_vec[j] * (j - current_pos + 1);
        }

        // Check if current price is better
        if (left + right < fuel) {
            optimum = current_pos;
            fuel = left + right;
        }
    }

    return fuel;
}

void test() {
    std::vector<int> test_data = read_file("inputs/input7_test.txt");

    assert(align_crabs(test_data) == 37);
    assert(align_crabs_linear(test_data) == 168);
}

int main() {
    test(); 

    std::vector<int> data = read_file("inputs/input7.txt");
    std::cout << align_crabs(data) << std::endl;
    std::cout << align_crabs_linear(data) << std::endl;

    return 0;
}
