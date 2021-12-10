#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <cassert>
#include <algorithm>

const int SCORE_BRACKET = 3;
const int SCORE_SQUARE = 57;
const int SCORE_CURLY = 1197;
const int SCORE_ANGLE = 25137;

const int SCORE_BRACKET_CLOSE = 1;
const int SCORE_SQUARE_CLOSE = 2;
const int SCORE_CURLY_CLOSE = 3;
const int SCORE_ANGLE_CLOSE = 4;

std::vector<std::string> read_file(const std::string &filename) {
    std::vector<std::string> data;

    std::ifstream in;
    in.open(filename);

    std::string line; 
    while (std::getline(in, line)) {
        data.push_back(line);
    }

    return data;
}

/* Calculate syntax error score for given lines of code */
int syntax_error_score(const std::vector<std::string> &data) {
    int score = 0;

    for (const std::string &line : data) {
        // Put opening brackets on a stack
        std::stack<char> brackets;

        for (char c : line) {
            if (c == '(' || c == '[' || c == '{' || c == '<') {
                // Push opening brackets
                brackets.push(c);
            } else {
                // Try to pop closing brackets
                char closing = brackets.top();
                closing = closing == '(' ? ')' :
                          closing == '[' ? ']' :
                          closing == '{' ? '}' :
                          closing == '<' ? '>' : closing;

                if (c == closing) {
                    // We found a matching closing bracket
                    brackets.pop();
                } else {
                    // This line is corrupted
                    switch (c) {
                        case ')':
                            score += SCORE_BRACKET;
                            break;
                        case ']':
                            score += SCORE_SQUARE;
                            break;
                        case '}':
                            score += SCORE_CURLY;
                            break;
                        case '>':
                            score += SCORE_ANGLE;
                            break;
                    }    
                    break;
                }
            }
        }
    }

    return score;
}

/* Calculate completion score and return the middle one */
long middle_score(const std::vector<std::string> &data) {
    std::vector<long> scores;

    for (const std::string &line : data) {
        // Put opening brackets on a stack
        std::stack<char> brackets;
        bool corrupted = false;

        for (char c : line) {
            if (c == '(' || c == '[' || c == '{' || c == '<') {
                // Push opening brackets
                brackets.push(c);
            } else {
                // Try to pop closing brackets
                char closing = brackets.top();
                closing = closing == '(' ? ')' :
                          closing == '[' ? ']' :
                          closing == '{' ? '}' :
                          closing == '<' ? '>' : closing;

                if (c == closing) {
                    // We found a matching closing bracket
                    brackets.pop();
                } else {
                    // Skip corrupted lines
                    corrupted = true;
                    break;
                }
            }
        }

        if (corrupted || brackets.empty()) {
            continue;
        }

        // Calculate score based on missing brackets
        long score = 0;
        while (!brackets.empty()) {
            char bracket = brackets.top(); 
            brackets.pop();

            score *= 5;
            switch (bracket) {
                case '(':
                    score += SCORE_BRACKET_CLOSE;
                    break;
                case '[':
                    score += SCORE_SQUARE_CLOSE;
                    break;
                case '{':
                    score += SCORE_CURLY_CLOSE;
                    break;
                case '<':
                    score += SCORE_ANGLE_CLOSE;
                    break;
            }
        }

        scores.push_back(score);
    }
    
    // Sort scores and return the middle one
    std::sort(scores.begin(), scores.end());
    return scores[scores.size() / 2];
}

void test() {
    std::vector<std::string> test_data = read_file("inputs/input10_test.txt");

    assert(syntax_error_score(test_data) == 26397);
    assert(middle_score(test_data) == 288957);
}

int main() {
    test();
    
    std::vector<std::string> data = read_file("inputs/input10.txt");
    std::cout << syntax_error_score(data) << std::endl;
    std::cout << middle_score(data) << std::endl;

    return 0;
}
