#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>

typedef std::pair<std::array<std::string, 10>, std::array<std::string, 4>> Entry;

const int NUM_OF_SEGMENTS[] = { 6, 2, 5, 5, 4, 5, 6, 3, 7, 6 };
const std::vector<int> DIGITS_OF_SEGMENTS[] = {
    {},
    {},
    { 1 },
    { 7 },
    { 4 },
    { 2, 3, 5 },
    { 0, 6, 9 },
    { 8 },
};

std::vector<Entry> read_file(const std::string &filename) {
    std::vector<Entry> data;

    std::ifstream in;
    in.open(filename);

    std::string word;
    while (in >> word) {
        // Read combinations before |
        std::array<std::string, 10> nums;
        for (int i = 0; i < 10; ++i) {
            std::sort(word.begin(), word.end());
            nums[i] = word;
            in >> word;
        }

        // Read combinations after |
        std::array<std::string, 4> result;
        for (int i = 0; i < 4; ++i) {
            in >> word;
            std::sort(word.begin(), word.end());
            result[i] = word;
        }

        data.push_back(std::make_pair(nums, result));
    }

    return data;
}

/* Count how many times do 1, 4, 7 and 8 appear in the result */
int num_of_appearances(const std::vector<Entry> &data) {
    int count = 0;

    for (const Entry e : data) {
        for (const std::string num : e.second) {
            // Check if number is 1, 4, 7 or 8
            if (num.size() == NUM_OF_SEGMENTS[1] || num.size() == NUM_OF_SEGMENTS[4] ||
                    num.size() == NUM_OF_SEGMENTS[7] || num.size() == NUM_OF_SEGMENTS[8]) {
                ++count;
            }
        }
    }

    return count;
}

/* Calculate intersection of two display representations */
std::string string_intersection(const std::string &a, const std::string &b) {
    std::stringstream result;

    for (const char ca : a) {
        for (const char cb : b) {
            if (ca == cb) {
                result << ca;
            }
        }
    }

    return result.str();
}

/* Determine wiring and return sum of results */
int outputs_sum(const std::vector<Entry> &data) {
    int sum = 0;

    for (const Entry e : data) {
        // Determine candidates for every digit
        std::array<std::vector<std::string>, 10> candidates;

        // This is a candidate for digits with the same number of display parts turned on
        for (const std::string combination : e.first) {
            for (int digit : DIGITS_OF_SEGMENTS[combination.size()]) {
                candidates[digit].push_back(combination);
            }
        }

        // 3 needs to have segments of 1
        std::erase_if(candidates[3], [candidates](std::string c) {
            std::string one = candidates[1][0];
            return one.size() != string_intersection(one, c).size();
        });

        // 9 needs to have segments of 3
        std::erase_if(candidates[9], [candidates](std::string c) {
            std::string three = candidates[3][0];
            return three.size() != string_intersection(three, c).size();
        });

        // 5 is contained in 9 and is not 3
        std::erase_if(candidates[5], [candidates](std::string c) {
            std::string nine = candidates[9][0];
            return c == candidates[3][0] || string_intersection(nine, c).size() != 5;
        });

        // 2 is not 3 or 5
        std::erase_if(candidates[2], [candidates](std::string c) {
            return c == candidates[3][0] || c == candidates[5][0];
        });

        // 0 contains 1 and is not 9
        std::erase_if(candidates[0], [candidates](std::string c) {
            std::string one = candidates[1][0];
            return c == candidates[9][0] || one.size() != string_intersection(one, c).size();
        });

        // 6 is not 0 or 9
        std::erase_if(candidates[6], [candidates](std::string c) {
            return c == candidates[0][0] || c == candidates[9][0];
        });

        // Increase sum by current result
        int temp = 0;
        for (const std::string num : e.second) {
            for (int i = 0; i <= 9; ++i) {
                if (candidates[i][0] == num) {
                    temp = 10 * temp + i;
                }
            }
        }

        sum += temp;
    }

    return sum;
}

void test() {
    std::vector<Entry> test_data = read_file("inputs/input8_test.txt");

    assert(num_of_appearances(test_data) == 26);
    assert(string_intersection("bcdf", "acfg") == "cf");
    assert(outputs_sum(test_data) == 61229);
}

int main() {
    test();

    std::vector<Entry> data = read_file("inputs/input8.txt");
    std::cout << num_of_appearances(data) << std::endl;
    std::cout << outputs_sum(data) << std::endl;

    return 0;
}
