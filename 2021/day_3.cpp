#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <utility>
#include <string>
#include <cmath>

/* Read input data */
std::vector<std::string> read_data(const std::string &filename) {
    std::vector<std::string> data; 

    std::ifstream in;
    in.open(filename);
    std::string line;
    while (in >> line) {
        data.push_back(line);
    }

    return data;
}

/* Calculate power consumption */
int calculate_power(const std::vector<std::string> &data) {
    // Get length of inputs
    auto length = data[0].size();
    
    std::vector<int> ones(length);

    // Calculate number of zeros and ones
    for (const std::string &input : data) {
        for (decltype(input.size()) i = 0; i < input.size(); ++i) {
            if (input[i] == '0') {
                --ones[i];
            } else if (input[i] == '1') {
                ++ones[i];
            }
        }
    }

    // Find gamma and epsilon rates
    int gamma = 0, epsilon = 0;
    for (int num_ones : ones) {
        if (num_ones >= 0) {
            gamma = 2 * gamma + 1;
            epsilon *= 2;
        } else {
            gamma *= 2;
            epsilon = 2 * epsilon + 1;
        }
    }

   return gamma * epsilon;
}

/* Calculate life support rating*/
int calculate_life_support(const std::vector<std::string> &data) {
    auto oxygen_candidates = data;
    auto co2_candidates = data;
    auto length = data[0].size();

    // Find oxygen rating
    for (decltype(data[0].size()) i = 0; i < length && oxygen_candidates.size(); ++i) {
        // Find most common bit value
        int ones = 0;
        for (decltype(oxygen_candidates.size()) j = 0; j < oxygen_candidates.size(); ++j) {
            if (oxygen_candidates[j][i] == '0') --ones;
            else if (oxygen_candidates[j][i]) ++ones;
        }

        char keep = ones >= 0 ? '1' : '0';

        // Filter candidates
        std::erase_if(oxygen_candidates, [i, keep](auto &a) {
            return a[i] != keep;
        });
    } 

    // Find CO2 rating
    for (decltype(data[0].size()) i = 0; i < length && co2_candidates.size(); ++i) {
        // Find most common bit value
        int ones = 0;
        for (decltype(co2_candidates.size()) j = 0; j < co2_candidates.size(); ++j) {
            if (co2_candidates[j][i] == '0') --ones;
            else if (co2_candidates[j][i]) ++ones;
        }

        char keep = ones < 0 ? '1' : '0';

        // Filter candidates
        std::erase_if(co2_candidates, [i, keep](auto &a) {
            return a[i] != keep;
        });
    } 
    
    return std::stoi(oxygen_candidates[0], nullptr, 2) * std::stoi(co2_candidates[0], nullptr, 2);
}

void test() {
    std::vector<std::string> test_data = read_data("inputs/input3_test.txt");
    assert(calculate_power(test_data) == 198);
    assert(calculate_life_support(test_data) == 230);
}

int main() {
    test();

    std::vector<std::string> data = read_data("inputs/input3.txt");
    std::cout << calculate_power(data) << std::endl;
    std::cout << calculate_life_support(data) << std::endl;

    return 0;
}
