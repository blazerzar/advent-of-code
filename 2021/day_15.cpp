#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <queue>

typedef std::vector<std::vector<int>> GRID;

struct Node {
    int x;
    int y;
    int cost;

    Node(int x, int y, int cost) : x(x), y(y), cost(cost) { } 
};

const int NEIGHBORS[4][2] = {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1}
};

GRID read_file(const std::string &filename) {
    GRID grid;

    std::ifstream in;
    in.open(filename);

    std::string line;
    while (std::getline(in, line)) {
        std::vector<int> grid_line;
        for (char c : line) {
            grid_line.push_back(c - '0');
        }
        grid.push_back(grid_line);
    }

    return grid;
}

/* Return length of path from top left to bottom right */
int shortest_path(const GRID &grid) {
    auto compare = [](const Node &a, const Node &b) {
        return a.cost > b.cost;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(compare)> queue(compare);

    // For part 2 we are solving a bigger grid
    int goal_y = grid.size() - 1;
    int goal_x = grid[goal_y].size() - 1;

    std::vector<std::vector<bool>> visited(
        goal_y + 1,
        std::vector<bool>(goal_x + 1)
    );

    // Start at top left
    queue.push(Node(0, 0, 0));

    while (!queue.empty()) {
        // Take the cheapest node
        Node next = queue.top();
        queue.pop();

        if (visited[next.y][next.x]) {
            continue;
        } 
        visited[next.y][next.x] = true;

        // Check if we are done
        if (next.y == goal_y && next.x == goal_x) {
            return next.cost;
        }

        // Generate all its neighbours
        for (int i = 0; i < 4; ++i) {
            int new_y = next.y + NEIGHBORS[i][0];
            int new_x = next.x + NEIGHBORS[i][1];

            // Check if neighbors is valid
            if (new_y >= 0 && new_y <= goal_y && new_x >= 0 && new_x <= goal_x) {
                queue.push(Node(new_x, new_y, next.cost + grid[new_y][new_x]));
            }
        }
    }
    
    return -1;
}

/* Return 5 times bigger grid */
GRID bigger_grid(const GRID &grid) {
    int size_y = grid.size();
    int size_x = grid[size_y - 1].size();
    std::vector<std::vector<int>> bigger(
        size_y * 5,
        std::vector<int>(size_x * 5)
    );
    
    for (decltype(bigger.size()) i = 0; i < bigger.size(); ++i) {
        for (decltype(bigger[i].size()) j = 0; j < bigger[i].size(); ++j) {
            int grid_y = i / size_y;
            int grid_x = j / size_x;
            bigger[i][j] = (grid[i % size_y][j % size_x] + grid_y + grid_x - 1) % 9 + 1;
        }
    }
    
    return bigger;
}

void test() {
    GRID test_data = read_file("inputs/input15_test.txt");

    assert(shortest_path(test_data) == 40);
    assert(shortest_path(bigger_grid(test_data)) == 315);
}

int main() {
    test();

    GRID data = read_file("inputs/input15.txt");
    std::cout << shortest_path(data) << std::endl;
    std::cout << shortest_path(bigger_grid(data)) << std::endl;

    return 0;
}

