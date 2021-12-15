#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <regex>
#include <utility>
#include <unordered_map>
#include <array>
#include <vector>

typedef std::unordered_map<std::string, std::string> Rules;

std::pair<std::string, Rules> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    std::string polymer;
    in >> polymer;

    Rules rules;

    std::string a;
    std::string b;
    std::string temp;
    while (in >> a) {
        in >> temp;
        in >> b;

        rules[a] = b;
    }
    
    return std::make_pair(polymer, rules);
}

/* Use rules to insert elements into polymer template */
std::string insert_elements(const std::string &polymer, Rules rules) {
    std::ostringstream result;

    // Add first element
    result << polymer[0];

    // Add remaining elements
    for (decltype(polymer.size()) i = 1; i < polymer.size(); ++i) {
        // Check if previous and current element form a rule
        auto it = rules.find(polymer.substr(i - 1, 2));
        if (it != rules.end()) {
            // Insert new element
            result << it->second;
        }
        result << polymer[i];
    }

    return result.str();
}

/* 
 * Return quantity of most common element subtracted by
 * quantity of least common element after running iterations
 * of element insertion
 */
int run_iterations(int iterations, std::string polymer, const Rules rules) {
    for (int i = 0; i < iterations; ++i) {
        polymer = insert_elements(polymer, rules);
    }

    // Count for every character
    std::array<int, 'Z' - 'A' + 1> counts{};
    // Find most and least common elements
    for (char c : polymer) {
        ++counts[c - 'A'];
    }  

    decltype(counts.size()) most_common = 0, least_common = 0;

    for (decltype(counts.size()) i = 1; i < counts.size(); ++i) {
        if (counts[i] > counts[most_common]) {
            most_common = i;
        }

        if (counts[least_common] == 0 || (counts[i] > 0 && counts[i] < counts[least_common])) {
            least_common = i;
        }
    }

    return counts[most_common] - counts[least_common];
} 

/* Run iterations more efficiently */
unsigned long long run_iterations_fast(int iterations, const std::string polymer, const Rules rules) {
    // Counter for every letter
    std::array<unsigned long long, 'Z' - 'A' + 1> counts{};

    // Tranform polymer string in map from pairs to count and update counter
    ++counts[polymer[0] - 'A'];
    std::unordered_map<std::string, unsigned long long> pairs;
    for (decltype(polymer.size()) i = 1; i < polymer.size(); ++i) {
        std::string pair = polymer.substr(i - 1, 2);
        auto it = pairs.find(pair);

        if (it != pairs.end()) {
            ++pairs[pair];
        } else {
            pairs[pair] = 1;
        }

        ++counts[polymer[i] - 'A'];
    }

    // Update pairs for every iteration
    for (int i = 0; i < iterations; ++i) {
        std::unordered_map<std::string, unsigned long long> pairs_new;

        // For every pair, check if it is in rules
        for (auto pair : pairs) {
            auto it = rules.find(pair.first);
            std::vector<std::pair<std::string, unsigned long long>> pairs_to_add;
            
            if (it != rules.end()) {
                // Every pair in rules is transformed into two new pairs
                char a = pair.first[0];
                char b = pair.first[1];
                std::string c = it->second;

                // Update letters counter
                counts[c[0] - 'A'] += pair.second;
                
                pairs_to_add.push_back(std::make_pair(a + c, pair.second));
                pairs_to_add.push_back(std::make_pair(c + b, pair.second));
            } else {
                // Pairs not present in rules remain the same
                pairs_to_add.push_back(pair);
            }

            // Add transformed pairs to a map
            for (auto pair_to_add : pairs_to_add) {
                auto it_pair = pairs_new.find(pair_to_add.first);

                if (it_pair != pairs_new.end()) {
                    pairs_new[pair_to_add.first] += pair_to_add.second;
                } else {
                    pairs_new[pair_to_add.first] = pair_to_add.second;
                }
            }
        }
        
        // Replace old pairs with transformed ones
        pairs = pairs_new;
    }

    decltype(counts.size()) most_common = 0, least_common = 0;

    // Find most and least common letters
    for (decltype(counts.size()) i = 1; i < counts.size(); ++i) {
        if (counts[i] > counts[most_common]) {
            most_common = i;
        }

        if (counts[least_common] == 0 || (counts[i] > 0 && counts[i] < counts[least_common])) {
            least_common = i;
        }
    }

    return counts[most_common] - counts[least_common];
}

void test() {
    std::pair<std::string, Rules> test_data = read_file("inputs/input14_test.txt");   

    assert(insert_elements(test_data.first, test_data.second) == "NCNBCHB"); 
    assert(run_iterations(10, test_data.first, test_data.second) == 1588); 
    assert(run_iterations_fast(10, test_data.first, test_data.second) == 1588); 
    assert(run_iterations_fast(40, test_data.first, test_data.second) == 2188189693529ull); 
}

int main() {
    test();

    std::pair<std::string, Rules> data = read_file("inputs/input14.txt");   
    std::cout << run_iterations(10, data.first, data.second) << std::endl;
    std::cout << run_iterations_fast(40, data.first, data.second) << std::endl;
    
    return 0;
}

