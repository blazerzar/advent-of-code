#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <utility>
#include <cassert>

#define BOARD_SIZE 5

typedef std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> Board;
typedef std::pair<std::vector<int>, std::vector<Board>> Data;

/* Read and parse input data */
Data read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    // Read bingo numbers
    std::vector<int> numbers;
    std::string numbers_line_str;
    std::getline(in, numbers_line_str);
    std::stringstream numbers_line;  
    numbers_line.str(numbers_line_str);
    
    int number;
    char tmp;
    while (numbers_line >> number) {
        numbers.push_back(number);
        
        // Read comma character
        numbers_line >> tmp; 
    }

    // Read boards
    std::vector<Board> boards;
    while (in >> number) {
        // Read new board
        Board new_board;
        for (decltype(new_board.size()) i = 0; i < BOARD_SIZE; ++i) {
            for (decltype(new_board[i].size()) j = 0; j < BOARD_SIZE; ++j) {
                new_board[i][j] = number; 

                // Don't read the first number of the next board
                if (i != BOARD_SIZE - 1 || j != BOARD_SIZE - 1) {
                    in >> number;
                }
            }
        }
        boards.push_back(new_board);
    }

    return std::make_pair(numbers, boards);
}

/* Return sum of all unmarked values */
int _unmarked_sum(const Board &board) {
    int sum = 0;
    for (const auto line : board) {
        for (const int value : line) {
            // Unmarked numbers are positive
            if (value > 0) {
                sum += value;
            }
        }
    }
    return sum;
}

/* Play bingo using given inputs */
std::pair<std::vector<int>::size_type, int>
bingo(const std::vector<int> &numbers, std::vector<Board> boards) {
    // Create counter for rows and columns
    std::vector<std::pair<std::array<int, BOARD_SIZE>, std::array<int, BOARD_SIZE>>> counts(boards.size());
    // [( # marked in i-th row, # marked in j-th column )]
    
    // Clear every number ...
    for (const int number : numbers) {
        // ... on every board
        for (decltype(boards.size()) k = 0; k < boards.size(); ++k) {
            Board &board = boards[k];

            for (decltype(board.size()) i = 0; i < board.size(); ++i) {
                for (decltype(board[i].size()) j = 0; j < board[i].size(); ++j) {
                    // Check for correct number and mark it if found
                    if (number == board[i][j]) {
                        board[i][j] = -1;
                        ++counts[k].first[i];
                        ++counts[k].second[j];

                        // Check if game is finished
                        if (counts[k].first[i] == BOARD_SIZE || counts[k].second[j] == BOARD_SIZE) {
                            return std::make_pair(k, _unmarked_sum(board) * number);
                        }
                    }
                }
            }
        } 
    }
    
    // ( index of winning board, score )
    return std::make_pair(-1, -1);
}

/* Play bingo until the last board wins */
int last_score(std::vector<int> numbers, std::vector<Board> boards) {
    while (boards.size() > 1) {
        auto result = bingo(numbers, boards);
        
        // Remove the winning board
        boards.erase(boards.begin() + result.first);
    }

    // Get the remaining board score
    return bingo(numbers, boards).second;
}

void test() {
    Data test_data = read_file("inputs/input4_test.txt");
    assert(bingo(test_data.first, test_data.second).second == 4512);
    assert(last_score(test_data.first, test_data.second) == 1924);
}

int main() {
    test();

    Data data = read_file("inputs/input4.txt");
    std::cout << bingo(data.first, data.second).second << std::endl;
    std::cout << last_score(data.first, data.second) << std::endl;

    return 0;
}
