#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cctype>

typedef std::unordered_map<std::string, std::vector<std::string>> Graph;

const std::string START("start");
const std::string END("end");

Graph read_file(const std::string &filename) {
    Graph data;
    
    std::ifstream in;
    in.open(filename);

    std::string line;
    while (std::getline(in, line)) {
        // Parse current edge end points
        decltype(line.size()) index = line.find('-');
        std::string a = line.substr(0, index);
        std::string b = line.substr(index + 1);

        // Create empty vectors for nodes if not yet present
        if (data.find(a) == data.end()) {
            data[a] = std::vector<std::string>();
        }
        if (data.find(b) == data.end()) {
            data[b] = std::vector<std::string>();
        }

        // Add edge to both nodes
        data[a].push_back(b);
        data[b].push_back(a);
    }

    return data;
}

/* Return number of paths from current node using given parameters */
int num_of_paths_rec(Graph graph, const std::string current, bool first,
        std::unordered_set<std::string> visited_lower, bool lower_twice) {
    // If we found end, stop 
    if (current == END) {
        return 1;
    } 

    // If current is lowercase, add it to visited_lower
    if (std::islower(current[0])) {
        visited_lower.insert(current);
    }

    int paths = 0;

    // Check all possible neighbors
    for (const std::string &neighbor : graph.at(current)) {
        if (std::islower(neighbor[0])) {
            if (visited_lower.find(neighbor) == visited_lower.end()) {
                // Only visit lowercase neighbor, if not yet visited
                paths += num_of_paths_rec(graph, neighbor, first, visited_lower, lower_twice);
            } else if (!first && !lower_twice && neighbor != START) {
                // Or if we can still visit one small cave twice
                paths += num_of_paths_rec(graph, neighbor, first, visited_lower, true);
            }
        } else {
            paths += num_of_paths_rec(graph, neighbor, first, visited_lower, lower_twice);
        }
    }
    
    return paths;
}

/* Return number of paths where we can visit small caves at most once */
int num_of_paths(const Graph &graph) {
    return num_of_paths_rec(graph, START, true, std::unordered_set<std::string>(), false);
}

/* Return number of paths where we can visit 1 small cave at most once and others once */
int num_of_paths_longer(const Graph &graph) {
    return num_of_paths_rec(graph, START, false, std::unordered_set<std::string>(), false);
}

void test() {
    Graph test_data1 = read_file("inputs/input12_test1.txt");
    Graph test_data2 = read_file("inputs/input12_test2.txt");
    Graph test_data3 = read_file("inputs/input12_test3.txt");
    
    assert(num_of_paths(test_data1) == 10);
    assert(num_of_paths(test_data2) == 19);
    assert(num_of_paths(test_data3) == 226);
    assert(num_of_paths_longer(test_data1) == 36);
    assert(num_of_paths_longer(test_data2) == 103);
    assert(num_of_paths_longer(test_data3) == 3509);
}

int main() {
    test();

    Graph data = read_file("inputs/input12.txt");
    std::cout << num_of_paths(data) << std::endl;
    std::cout << num_of_paths_longer(data) << std::endl;

    return 0;
}
