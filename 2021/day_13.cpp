#include <iostream>
#include <fstream>
#include <cassert>
#include <utility>
#include <vector>
#include <unordered_set>
#include <regex>
#include <string>

struct Point {
    int x;
    int y;

    Point(int x, int y) : x(x), y(y) { }

    bool operator==(const Point &point) const {
        return this->x == point.x && this->y == point.y;
    }
   
    struct HashFunction {
        size_t operator()(const Point& point) const {
            size_t x_hash = std::hash<int>()(point.x);
            size_t y_hash = std::hash<int>()(point.y) << 1;
            return x_hash ^ y_hash;
        }
    };
};

typedef std::unordered_set<Point, Point::HashFunction> Grid;

struct Fold {
    char direction; 
    int coordinate;

    Fold (char d, int c) : direction(d), coordinate(c) { }
};

std::pair<Grid, std::vector<Fold>>
read_file(const std::string &filename) {
    Grid grid;
    std::vector<Fold> folds;
    
    std::ifstream in;
    in.open(filename);
    std::string line; 

    std::regex point_regex("(\\d+),(\\d+)");
    std::regex fold_regex("fold along (x|y)=(\\d+)");
    std::smatch match;

    while (std::getline(in, line)) {
        if (std::regex_match(line, match, point_regex)) {
            grid.insert(Point(std::stoi(match[1]), std::stoi(match[2])));
        } else if (std::regex_match(line, match, fold_regex)) {
            folds.push_back(Fold(match.str(1)[0], std::stoi(match[2])));
        }
    }
   
    return std::make_pair(grid, folds);
}

/* Return number of visible dots after folding using [Fold] once*/
Grid fold(const Grid &grid, Fold f) {
    Grid folded; 

    // Copy dots under fold and transfot those above
    for (const Point p : grid) {
        if (f.direction == 'x' && p.x < f.coordinate ||
                f.direction == 'y' && p.y < f.coordinate) {
            folded.insert(p);
        } else if (f.direction == 'x') {
            int new_x = f.coordinate - (p.x - f.coordinate);
            folded.insert(Point(new_x, p.y));
        } else if (f.direction =='y') {
            int new_y = f.coordinate - (p.y - f.coordinate);
            folded.insert(Point(p.x, new_y));
        }
    }

    return folded;
}

/* Print out a grid visualization */
void visualize_grid(const Grid &grid) {
    // Find max y and x value
    int max_y = 0, max_x = 0;
    for (const Point &p : grid) {
        if (p.y > max_y) max_y = p.y;
        if (p.x > max_x) max_x = p.x;
    }

    // Draw grid
    for (int y = 0; y <= max_y; ++y) {
        for (int x = 0; x <= max_x; ++x) {
            std::cout << (grid.find(Point(x, y)) == grid.end() ? '.' : '#'); 
        }
        std::cout << std::endl;
    }
}

/* Fold grid using all folds */
void all_folds(Grid grid, std::vector<Fold> &folds) {
    for (Fold f : folds) {
        grid = fold(grid, f); 
    }
    visualize_grid(grid);
}

void test() {
    std::pair<Grid, std::vector<Fold>> test_data = read_file("inputs/input13_test.txt");
    
    assert(fold(test_data.first, test_data.second[0]).size() == 17);
}

int main() {
    test();

    std::pair<Grid, std::vector<Fold>> data = read_file("inputs/input13.txt");
    std::cout << fold(data.first, data.second[0]).size() << std::endl;
    all_folds(data.first, data.second);
     
    return 0;
}
