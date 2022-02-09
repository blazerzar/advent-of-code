#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <map>
#include <array>
#include <algorithm>
#include <iomanip>
#include <cassert>

struct Position {
    int x;
    int y;
    int z;
};

/* To use Position in a set */
inline bool operator<(const Position &lhs, const Position &rhs) {
    if (lhs.x != rhs.x) return lhs.x < rhs.x;
    if (lhs.y != rhs.y) return lhs.y < rhs.y;
    return lhs.z < rhs.z;
}

typedef Position Beacon;

struct Scanner {
    std::vector<Beacon> beacons;
    std::vector<std::vector<int>> distances;
};

typedef std::pair<int, int> Match;

struct Overlap {
    // Scanner indices
    int first;
    int second;
    std::vector<Match> matches;
};

struct CoordsOrder {
    int x;
    int y;
    int z;
};

/* Represent a linear transformation */
struct Transformation {
    int x;
    int y;
    int z;
    int mirror;
    Position t;

    Position operator()(const Position &p) const {
        int pos[] = { p.x, p.y, p.z };
        return {
            pos[x] * (mirror & 1 ? -1 : 1) + t.x,
            pos[y] * (mirror & 2 ? -1 : 1) + t.y,
            pos[z] * (mirror & 4 ? -1 : 1) + t.z
        };
    }
    
    /* Create new transformation that is inverse to this one */
    Transformation inverse() const {
        int pos[] = { t.x, t.y, t.z };
        // Transpose 'matrix'
        int xx = y == 0 ? 1 : z == 0 ? 2 : 0;
        int yy = x == 1 ? 0 : z == 1 ? 2 : 1;
        int zz = x == 2 ? 0 : y == 2 ? 1 : 2;
    
        // Rearrange mirror bits using new 'matrix'
        int pos_m[] = { mirror & 1, (mirror & 2) >> 1, (mirror & 4) >> 2 };
        int m = 1 * pos_m[xx] + 2 * pos_m[yy] + 4 * pos_m[zz];
        
        return { xx, yy, zz, m, {
            // We will subtract translation multiplied by 'matrix' inverse
            -pos[xx] * (m & 1 ? -1 : 1),
            -pos[yy] * (m & 2 ? -1 : 1),
            -pos[zz] * (m & 4 ? -1 : 1)
        } };
    }
};

/* Composition of 2 transformations */
Transformation operator+(const Transformation &lhs, const Transformation &rhs) {
    // Use arrays to 'rotate' first rotation and translation into the second CS
    int pos[] = { lhs.x, lhs.y, lhs.z };
    int pos_t[] = { lhs.t.x, lhs.t.y, lhs.t.z };
    
    int pos_m[] = { lhs.mirror & 1, (lhs.mirror & 2) >> 1, (lhs.mirror & 4) >> 2 };
    // First 'rotate' mirror into the second coordinate system ...
    int mirror = 1 * pos_m[rhs.x] + 2 * pos_m[rhs.y] + 4 * pos_m[rhs.z];
    // ... then we multiply element-wise using XOR
    mirror ^= rhs.mirror;
    
    return { pos[rhs.x], pos[rhs.y], pos[rhs.z], mirror, {
        pos_t[rhs.x] * (rhs.mirror & 1 ? -1 : 1) + rhs.t.x,
        pos_t[rhs.y] * (rhs.mirror & 2 ? -1 : 1) + rhs.t.y,
        pos_t[rhs.z] * (rhs.mirror & 4 ? -1 : 1) + rhs.t.z
    } };
}

const std::array<CoordsOrder, 6> XYZ_COMBINATIONS{{
    { 0, 1, 2 }, 
    { 0, 2, 1 }, 
    { 1, 0, 2 }, 
    { 1, 2, 0 }, 
    { 2, 0, 1 }, 
    { 2, 1, 0 } 
}};

/* Calculate distance between 2 positions */
int distance(const Position &a, const Position &b, bool manhattan = false) {
    if (manhattan) {
        return std::abs(b.x - a.x) + std::abs(b.y - a.y) + std::abs(b.z - a.z);
    } else {
        return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) + (b.z - a.z) * (b.z - a.z);
    }
}

/* Compute distances between every combination of 2 beacons */
void compute_matrix(Scanner &scanner) {
    auto &b = scanner.beacons;
    scanner.distances = std::vector<std::vector<int>>(b.size(), std::vector<int>(b.size()));

    for (decltype(b.size()) i = 0; i < b.size(); ++i) {
        scanner.distances[i][i] = 0;
        for (decltype(b.size()) j = i + 1; j < b.size(); ++j) {
            int d = distance(b[i], b[j]);
            scanner.distances[i][j] = d;
            scanner.distances[j][i] = d;
        }
    }
}

/* Find pairs of beacons with at least 12 distances in common */
std::vector<Match> find_matches(const Scanner &first, const Scanner &second) {
    std::vector<std::pair<int, int>> matches;

    for (decltype(first.beacons.size()) i = 0; i < first.beacons.size(); ++i) {
        std::set<int> first_distances(first.distances[i].begin(), first.distances[i].end()); 
        for (decltype(second.beacons.size()) j = 0; j < second.beacons.size(); ++j) {
            std::set<int> second_distances(second.distances[j].begin(), second.distances[j].end()); 
            std::set<int> common_distances;
            std::set_intersection(first_distances.begin(), first_distances.end(),
                                  second_distances.begin(), second_distances.end(),
                                  std::inserter(common_distances, common_distances.begin()));

            if (common_distances.size() >= 12) {
                matches.push_back({ i, j });
            }
        }
    }

    return matches;
}

/* Find pairs of scanners with at least 12 beacons in common */
std::vector<Overlap> find_overlaps(const std::vector<Scanner> &scanners) {
    std::vector<Overlap> overlaps;

    for (decltype(scanners.size()) i = 0; i < scanners.size(); ++i) {
        for (decltype(scanners.size()) j = i + 1; j < scanners.size(); ++j) {
            auto matches = find_matches(scanners[i], scanners[j]);

            if (matches.size() >= 12) {
                overlaps.push_back({ (int) i, (int) j, matches });
            }
        }
    }

    return overlaps;
}

/* Return transformation from second to first scanner's coordinate system */
Transformation get_relative_position(
        const Scanner &first,
        const Scanner &second,
        const std::vector<Match> &matches) {
    
    // We will permute these coordinates
    int second_pos[] = {
        second.beacons[matches[0].second].x,
        second.beacons[matches[0].second].y,
        second.beacons[matches[0].second].z
    };

    // Check every possible xyz -> xyz mapping (permutations)
    for (auto [x, y, z] : XYZ_COMBINATIONS) {
        // Check every possible axis mirroring
        for (int mirror = 0; mirror < 8; ++mirror) {
            // Calculate difference between first and second coordinate systems
            int dx = first.beacons[matches[0].first].x - second_pos[x] * (mirror & 1 ? -1 : 1);
            int dy = first.beacons[matches[0].first].y - second_pos[y] * (mirror & 2 ? -1 : 1);
            int dz = first.beacons[matches[0].first].z - second_pos[z] * (mirror & 4 ? -1 : 1);

            // Check if differences between all matches are the same
            bool result = true;
            for (decltype(matches.size()) i = 1; i < matches.size() && result; ++i) {
                // For permuting
                int second_poss[] = {
                    second.beacons[matches[i].second].x,
                    second.beacons[matches[i].second].y,
                    second.beacons[matches[i].second].z
                };

                int dxx = first.beacons[matches[i].first].x - second_poss[x] * (mirror & 1 ? -1 : 1);
                int dyy = first.beacons[matches[i].first].y - second_poss[y] * (mirror & 2 ? -1 : 1);
                int dzz = first.beacons[matches[i].first].z - second_poss[z] * (mirror & 4 ? -1 : 1);

                if (dx != dxx || dy != dyy || dz != dzz) {
                    result = false;
                } 
            }

            // We have found the correct combination
            if (result) {
                return { x, y, z, mirror, { dx, dy, dz } };
            }
        }
    }

    return { 0, 1, 2, 0, { 0, 0, 0 } };
}

/* Find total number of beacons taking overlapping scanners into account */
int number_of_beacons(const std::vector<Scanner> &scanners,
                      std::map<int, Transformation> &scanners_relative) {
    // Find pairs of scanners that have enough matches
    std::vector<Overlap> overlaps = find_overlaps(scanners);

    // Transformation from each scanner to scanner 0
    scanners_relative[0] = { 0, 1, 2, 0, { 0, 0, 0 } };    // Identity transformation
    
    // Loop until all transformation are found
    while (scanners_relative.size() < scanners.size()) {
        // Find transformation between overlapping scanners
        for (auto &[i, j, matches] : overlaps) {
            // Skip match if we cannot locate scanner yet
            auto res_i = scanners_relative.find(i); 
            auto res_j = scanners_relative.find(j); 
            if (res_i == scanners_relative.end() && res_j == scanners_relative.end()) {
                continue;
            }

            // Get transformation from j to i
            auto trans_to_i = get_relative_position(scanners[i], scanners[j], matches);

            // Calculate transformation from scanner j to scanner 0
            if (i == 0) {
                // We have transformation from 0 to j, just inverse it
                scanners_relative[j] = trans_to_i;
            } else if (j == 0) {
                // We already have transformation to 0
                scanners_relative[i] = trans_to_i.inverse(); 
            } else if (res_i != scanners_relative.end()) {
                // We have transformation to known scanner i
                scanners_relative[j] = trans_to_i + res_i->second; 
            } else if (res_j != scanners_relative.end()) {
                // We have transformation from known scanner j
                scanners_relative[i] = trans_to_i.inverse() + res_j->second;
            } else {
                std::cout << "Cannot locate scanner!\n";
            }
        }
    }
    
    // Transform all beacons to coordinate system of scanner 0
    std::set<Beacon> beacons;
    for (decltype(scanners.size()) i = 0; i < scanners.size(); ++i) {
        for (const Beacon &beacon : scanners[i].beacons) {
            beacons.insert(scanners_relative[i](beacon));
        }
    }

    return beacons.size();
}

/* Find largest Manhattan distance between 2 scanners */
int largest_scanners_distance(std::map<int, Transformation> scanners) {
    int max = 0;

    // Check distance between every pair to find max
    for (decltype(scanners.size()) i = 0; i < scanners.size(); ++i) {
        Position pos_i = scanners[i]({ 0, 0, 0 });
        for (decltype(scanners.size()) j = i + 1; j < scanners.size(); ++j) {
            Position pos_j = scanners[j]({ 0, 0, 0 });
            int d = distance(pos_i, pos_j, true);
            max = d > max ? d : max;
        }
    }
    
    return max;
}

/* Print 2D matrix, for debugging */
void print_matrix(const std::vector<std::vector<int>> &matrix) {
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = 0; j < matrix[0].size(); ++j) {
            std::cout << std::setfill(' ') << std::setw(4) << matrix[i][j] << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

std::vector<Scanner> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    std::string line;
    std::vector<Scanner> scanners;

    while (std::getline(in, line)) {
        Scanner scanner;

        while (std::getline(in, line) && line.size() > 1) {
            // Split line by commas
            int comma_1 = line.find_first_of(',');
            int comma_2 = line.find_last_of(',');
            scanner.beacons.push_back({
                std::stoi(line.substr(0, comma_1)),
                std::stoi(line.substr(comma_1 + 1, comma_2 - comma_1 - 1)),
                std::stoi(line.substr(comma_2 + 1)),
            });
        }

        compute_matrix(scanner);
        scanners.push_back(scanner);
    }

    return scanners;
}

void test() {
    std::vector<Scanner> scanners = read_file("inputs/input19_test.txt");
    std::map<int, Transformation> scanners_relative;
    assert(number_of_beacons(scanners, scanners_relative) == 79);
    assert(largest_scanners_distance(scanners_relative) == 3621);
}

int main() {
    test();

    std::vector<Scanner> scanners = read_file("inputs/input19.txt");
    std::map<int, Transformation> scanners_relative;
    std::cout << number_of_beacons(scanners, scanners_relative) << '\n';
    std::cout << largest_scanners_distance(scanners_relative) << '\n';

    return 0;
}

