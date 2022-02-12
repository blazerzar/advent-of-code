#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <algorithm>
#include <cassert>

std::map<char, int> ENERGIES = {
    { 'A', 1 }, { 'B', 10 }, { 'C', 100 }, { 'D', 1000 }
};

struct Burrow {
    std::vector<char> hallway;
    std::map<char, std::deque<char>> rooms;

    /* Return true if all amphipods are in correct rooms */
    bool is_final() const {
        for (char c : hallway) {
            if (c != '.') return false;
        }

        for (char c = 'A'; c <= 'D'; ++c) {
            for (char cc : rooms.at(c)) {
                if (cc != c) return false;
            }
        }

        return true;
    }
};

bool operator<(const Burrow &lhs, const Burrow &rhs) {
    if (lhs.hallway != rhs.hallway) return lhs.hallway < rhs.hallway;
    return lhs.rooms < rhs.rooms;
}

struct State {
    Burrow situation;
    int energy;
};

bool operator<(const State &lhs, const State &rhs) {
    return lhs.energy > rhs.energy;
}

/* Find all possible moves and add the to priority queue */
void generate_moves(const State &state, std::priority_queue<State> &q,
                    const std::set<Burrow> &visited, int room_size) {
    Burrow situation = state.situation;

    // Try moves from rooms to hallway
    for (char room = 'A'; room <= 'D'; ++room) {
        // Check if there are any amphipods to move
        if (situation.rooms.at(room).empty()) continue;

        char amphipod = situation.rooms.at(room).back();
        int room_index = (room - 'A') * 2 + 2;

        // Do not move amphipod from the correct room
        if (room == amphipod && situation.rooms.at(room).front() == room) continue;

        // Move left (change = -1) and right (change = 1) from the room
        for (int change = -1; change <= 1; change += 2) {
            for (decltype(situation.hallway.size()) pos = room_index + change;
                    pos < situation.hallway.size() && pos >= 0 &&
                    situation.hallway[pos] == '.'; pos += change) {
                // Move into room if there is space
                if (pos == 2 || pos == 4 || pos == 6 || pos == 8) continue;

                // Create new situation
                Burrow new_situation = situation;
                new_situation.rooms[room].pop_back();
                new_situation.hallway[pos] = amphipod;

                // Calculate energy
                int path_length = (std::abs((int) pos - room_index) + room_size - new_situation.rooms.at(room).size());

                // Add to queue
                if (visited.find(new_situation) == visited.end()) {
                    q.push({ new_situation, state.energy + path_length * ENERGIES.at(amphipod) });
                }
            }
        }
    }

    // Try moves from hallway into correct rooms
    for (decltype(situation.hallway.size()) hall = 0; hall < situation.hallway.size(); ++hall) {
        // Check if there is amphipod in the current hallway position
        if (situation.hallway[hall] == '.') continue;

        char amphipod = situation.hallway[hall];

        // Check if amphipod's room is available
        bool available = true;
        for (char c : situation.rooms.at(amphipod)) {
            available = available && c == amphipod;
        }
        if (!available) continue;

        // Check if path to room is clear
        int room_index = (amphipod - 'A') * 2 + 2;
        if (hall < room_index) {
            for (decltype(situation.hallway.size()) pos = hall + 1; pos < room_index; ++pos) {
                available = available && situation.hallway[pos] == '.';
            }
        } else {
            for (decltype(situation.hallway.size()) pos = hall - 1; pos > room_index; --pos) {
                available = available && situation.hallway[pos] == '.';
            }
        }
        if (!available) continue;

        // Move is valid, create new situation and calculate energy
        Burrow new_situation = situation;
        new_situation.rooms[amphipod].push_back(amphipod);
        new_situation.hallway[hall] = '.';
        int path_length = (std::abs((int) hall - room_index) + room_size - situation.rooms.at(amphipod).size());

        if (visited.find(new_situation) == visited.end()) {
            q.push({ new_situation, state.energy + path_length * ENERGIES.at(amphipod) });
        }
    }
}

/* Find energy cost for organizing amphipods into corrent rooms */
int organize_amphipods(const Burrow &start, int room_size) {
    // PQ for Dijkstra
    std::priority_queue<State> q;
    std::set<Burrow> visited;
    q.push({ start, 0 });

    while (!q.empty()) {
        // Take cheapest situation
        State state = q.top();
        q.pop();

        // Skip states to which we have already found the shortest path
        if (visited.find(state.situation) != visited.end()) continue;

        visited.insert(state.situation);

        // Check if situation is final
        if (state.situation.is_final()) {
            return state.energy;
        }

        // Find all possible moves and add them to queue
        generate_moves(state, q, visited, room_size);
    }

    return -1;
}

/* Insert amphipods hidden by the folding */
void unfold(Burrow &situation) {
    std::map<char, std::vector<char>> to_add = {
        { 'A', { 'D', 'D' } },
        { 'B', { 'C', 'B' } },
        { 'C', { 'B', 'A' } },
        { 'D', { 'A', 'C' } }
    };
    for (char room = 'A'; room <= 'D'; ++room) {
        to_add.at(room).push_back(situation.rooms.at(room).front());
        situation.rooms.at(room).pop_front();

        for (char amphipod : to_add.at(room)) {
            situation.rooms.at(room).push_front(amphipod);
        }
    }
}

Burrow read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);
    Burrow situation;

    std::string line;

    // Skip first line
    in >> line >> line;

    // Read hallway
    for (decltype(line.size()) i = 1; i < line.size() - 1; ++i) {
        situation.hallway.push_back(line[i]);
    }

    // Skip one line
    in >> line;

    // Create empty rooms
    situation.rooms['A'] = std::deque<char>();
    situation.rooms['B'] = std::deque<char>();
    situation.rooms['C'] = std::deque<char>();
    situation.rooms['D'] = std::deque<char>();

    // Read side rooms
    while (std::getline(in, line) && line[3] != '#') {
        for (decltype(line.size()) i = 3; i < 10; i += 2) {
            situation.rooms['A' + (i - 3) / 2].push_front(line[i]);
        }
    }

    return situation;
}

void test() {
    Burrow situation = read_file("inputs/input23_test.txt");
    assert(organize_amphipods(situation, 2) == 12521);
    unfold(situation);
    assert(organize_amphipods(situation, 4) == 44169);
}

int main() {
    test();

    Burrow situation = read_file("inputs/input23.txt");
    std::cout << organize_amphipods(situation, 2) << '\n';
    unfold(situation);
    std::cout << organize_amphipods(situation, 4) << '\n';

    return 0;
}
