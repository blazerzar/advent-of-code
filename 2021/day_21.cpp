#include <iostream>
#include <fstream>
#include <utility>
#include <cassert>

/* Deterministic die from the problem */
struct Die {
    int curr = 0;
    int rolls = 0;

    int roll() {
        curr = (curr) % 100 + 1;
        ++rolls;
        return curr;
    }
};

/* Return new position */
int move(int position, int steps) {
    return (position + steps - 1) % 10 + 1;
}

/* Play a practice game using deterministic die */
int practice_game(int first, int second) {
    int positions[] = { first, second };
    int scores[] = { 0, 0 };
    int turn = 0;
    Die die;
    
    while (scores[0] < 1000 && scores[1] < 1000) {
        int roll = die.roll() + die.roll() + die.roll(); 
        scores[turn] += positions[turn] = move(positions[turn], roll);
        turn = 1 - turn;
    }

    int winner = scores[1] >= 1000;
    return die.rolls * scores[1 - winner];
}

/* Recursive function for playing the game */
std::pair<long, long> play_game(int first, int second, int first_score, int second_score,
                                int turn, int rolls, long memo[11][11][21][21][2][4][2]) {
    // Check if solution is available
    if (memo[first][second][first_score][second_score][turn][rolls][0] ||
            memo[first][second][first_score][second_score][turn][rolls][1]) {
        return {
            memo[first][second][first_score][second_score][turn][rolls][0],
            memo[first][second][first_score][second_score][turn][rolls][1]
        };
    }
    
    std::pair<long, long> res;

    // If player rolled 3 times, execute the move 
    if (rolls < 3) {
        long winnings[] = { 0, 0 };
        // Split game into 3 universes
        for (int roll = 1; roll <= 3; ++roll) {
            if (turn) {
                int new_second = move(second, roll);
                res = play_game(first, new_second, first_score, second_score, turn, rolls + 1, memo);
                // Save solution
                memo[first][new_second][first_score][second_score][turn][rolls + 1][0] = res.first;
                memo[first][new_second][first_score][second_score][turn][rolls + 1][1] = res.second;
            } else {
                int new_first = move(first, roll);
                res = play_game(new_first, second, first_score, second_score, turn, rolls + 1, memo);
                // Save solution
                memo[new_first][second][first_score][second_score][turn][rolls + 1][0] = res.first;
                memo[new_first][second][first_score][second_score][turn][rolls + 1][1] = res.second;
            }

            winnings[0] += res.first;
            winnings[1] += res.second;
        }

        return { winnings[0], winnings[1] };
    } else {
        // Change player and add score
        if (turn) {
            int new_second_score = second_score + second;

            // Check if player has won
            if (new_second_score >= 21) return { 0, 1 };   

            res = play_game(first, second, first_score, new_second_score, 1 - turn, 0, memo);
            // Save solution
            memo[first][second][first_score][new_second_score][1 - turn][0][0] = res.first;
            memo[first][second][first_score][new_second_score][1 - turn][0][1] = res.second;
        } else {
            int new_first_score = first_score + first;

            // Check if player has won
            if (new_first_score >= 21) return { 1, 0 };   

            res = play_game(first, second, new_first_score, second_score, 1 - turn, 0, memo);
            // Save solution
            memo[first][second][new_first_score][second_score][1 - turn][0][0] = res.first;
            memo[first][second][new_first_score][second_score][1 - turn][0][1] = res.second;
        }

        return res;
    }
}

/* Return number of wins for each player in all universes together */
std::pair<long, long> real_game(int first, int second) {
    // Create table for memoization
    // first * second * first_score * second_score * turn * rolls * 2 (result)
    long memo[11][11][21][21][2][4][2] = {};

    return play_game(first, second, 0, 0, 0, 0, memo);
}

std::pair<int, int> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);
    int first, second;

    // Skip text
    for (int i = 0; i < 28; ++i) in.get();
    in >> first;
    for (int i = 0; i < 28; ++i) in.get();
    in >> second;
    return { first, second };
}

void test() {
    auto [first, second] = read_file("inputs/input21_test.txt");
    assert(move(7, 5) == 2);
    assert(practice_game(first, second) == 739785);
    
    auto res = real_game(first, second);
    assert(res.first == 444356092776315l && res.second == 341960390180808l);
}

int main() {
    test();

    auto [first, second] = read_file("inputs/input21.txt");
    std::cout << practice_game(first, second) << '\n';
    auto res = real_game(first, second);
    std::cout << std::max(res.first, res.second) << '\n';
     
    return 0;
}

