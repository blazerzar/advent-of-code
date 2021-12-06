#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <array>
#include <numeric>

/* Reads the initial states of starting fish */
std::vector<int> read_file(const std::string &filename) {
    std::vector<int> data;

    std::ifstream in;
    in.open(filename);
    int state;
    char temp;

    while (in >> state) {
        data.push_back(state); 

        // Read separating comma
        in >> temp;
    }
    
    return data;
}

/* Return a string of the current state */
std::string _state_string(const std::vector<int> &fish) {
    std::stringstream s;
    std::copy(fish.begin(), fish.end() - 1, std::ostream_iterator<int>(s, ","));

    // Add the last element
    s << *(fish.end() - 1);

    return s.str();
}

/* Simulate fish reproduction */
std::vector<int>::size_type num_of_fish(std::vector<int> fish, int days) {
    for (int i = 0; i < days; ++i) {
        int newFish = 0;

        // Decrement all fish
        for (int &f : fish) {
            if (--f < 0) {
                f = 6;
                ++newFish;
            }
        }

        // Add new fish
        for (int j = 0; j < newFish; ++j) {
            fish.push_back(8);
        }
    }

    return fish.size();
}

/* Transform list of states in an array of counters */
std::array<unsigned long long, 9> transform_state(const std::vector<int> &fish) {
    std::array<unsigned long long, 9> states{};
    
    for (const int f : fish) {
        ++states[f];
    }
    
    return states;
}

/* Calculate number of fish using array of counters */
unsigned long long num_of_fish_better(const std::vector<int> &fish, int days) {
    std::array<unsigned long long, 9> states = transform_state(fish);

    for (int i = 0; i < days; ++i) {
        std::array<unsigned long long, 9> newStates{};

        // All states decrease for every fish
        for (int j = 0; j < 8; ++j) {
            newStates[j] = states[j + 1];
        }

        // Fish at state 0 are reset to 6 and ...
        newStates[6] += states[0];
        // ... we create one new fish for every one
        newStates[8] += states[0];

        states = newStates;
    }

    return std::accumulate(states.cbegin(), states.cend(), 0ull, std::plus<unsigned long long>());
}

void test() {
    std::vector<int> test_data = read_file("inputs/input6_test.txt");
    assert(num_of_fish(test_data, 80) == 5934);
    assert(num_of_fish_better(test_data, 256) == 26984457539);
}

int main() {
    test(); 

    std::vector<int> data = read_file("inputs/input6.txt");
    std::cout << num_of_fish(data, 80) << std::endl;
    std::cout << num_of_fish_better(data, 256) << std::endl;

    return 0;
}

