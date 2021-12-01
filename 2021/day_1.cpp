#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

/* Read and parse input file <filename> */
std::vector<int> read_file(std::string filename) {
    std::ifstream fin; 
    fin.open(filename);
    
    int input;
    std::vector<int> inputs;
    while (fin >> input) {
        inputs.push_back(input);
    }

    return inputs;
}

/* Count number of increases of consecutive measurements */
int num_of_increases(const std::vector<int> input) {
    int increases = 0;
    for (decltype(input.size()) i = 1; i < input.size(); ++i) {
        increases += input[i] > input[i - 1];
    }
    return increases;
}

/* Count number of increases of consecutive window sums */
int num_of_increases_window(const std::vector<int> input) {
    int increases = 0;

    // We need at least 3 measurements to calculate the first window
    if (input.size() >= 3) {
        int prev_window = input[0] + input[1] + input[2];
        
        // Calculate the next window sum
        for (decltype(input.size()) i = 1; i + 2 < input.size(); ++i) {
            int curr_window = input[i] + input[i + 1] + input[i + 2];  
            increases += curr_window > prev_window;

            // Save current window sum for next iteration
            prev_window = curr_window;
        }
    }

    return increases;
}

/* Test small cases */
void test() {
    std::vector<int> test_inputs = read_file("inputs/input1_test.txt");
    assert(num_of_increases(test_inputs) == 7);
    assert(num_of_increases_window(test_inputs) == 5);
}

int main() {
    test(); 

    std::vector<int> inputs = read_file("inputs/input1.txt");
    std::cout << num_of_increases(inputs) << std::endl;
    std::cout << num_of_increases_window(inputs) << std::endl;

    return 0;
}
