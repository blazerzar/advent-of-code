#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct Parameters {
    int a;
    int b;
    int c;
};

/* Read parameters for every interation */
std::vector<Parameters> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    std::vector<Parameters> parameters;
    std::string line;
    int a, b, c;

    while (std::getline(in, line)) {
        for (int i = 0; i < 3; ++i) std::getline(in, line);
        in >> line >> line >> a;
        in >> line >> line >> b;
        for (int i = 0; i < 10; ++i) std::getline(in, line);
        in >> line >> line >> c;
        for (int i = 0; i < 3; ++i) std::getline(in, line);

        parameters.push_back({ a, b, c});
    }

    return parameters;
}

/*
if (z[i-1] % 26 + b) == input[i]:
    z[i] = z[i-1] / a
else:
    z[i] = (z[i-1] / a) * 26 + input[i] + c
*/

/* Find largest or smallest model number that is valid */
long find_valid_model_num(const std::vector<Parameters> &parameters, bool max_model) {
    // For every z_i we save model number that generated that z_i
    std::unordered_map<int, long> zs;
    zs[0] = 0;

    for (int iteration = 0; iteration < 14; ++iteration) {
        auto [a, b, c] = parameters.at(iteration);
        std::unordered_map<int, long> new_zs;

        // Calculate every possible z in the next iteration
        for (auto [prev_z, model] : zs) {
            for (int digit = 1; digit <= 9; ++digit) {
                // Add next digit to the model number
                long new_model = model * 10 + digit;

                int z;
                if (prev_z % 26 + b == digit) {
                    z = prev_z / a;
                } else {
                    z = (prev_z / a) * 26 + digit + c;
                }
                
                // If we already got to this z, save the max/min one
                if (auto it = new_zs.find(z); it != new_zs.end()) {
                    new_zs[z] = max_model ?
                        std::max(it->second, new_model) :
                        std::min(it->second, new_model);
                } else {
                    new_zs[z] = new_model; 
                }
            }
        }
        
        zs = std::move(new_zs);
    }
    
    // Return model number that generated z=0 on the last iteration
    return zs[0];
}

int main() {
    
    std::vector<Parameters> parameters = read_file("inputs/input24.txt"); 
    std::cout << find_valid_model_num(parameters, true) << '\n';    
    std::cout << find_valid_model_num(parameters, false) << '\n';    
    
    return 0;
}

