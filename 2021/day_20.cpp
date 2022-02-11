#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <array>
#include <numeric>
#include <algorithm>
#include <cassert>

typedef std::vector<std::string> Image;
typedef std::string EnhanceAlgo;

const std::array<std::pair<int, int>, 9> WINDOW = {{
    { -1, -1 }, { -1, 0 }, { -1, 1 },
    {  0, -1 }, {  0, 0 }, {  0, 1 },
    {  1, -1 }, {  1, 0 }, {  1, 1 }
}};

/* Return number of the window centered at given coordinates */
int get_window_number(const Image &image, int x, int y) {
    int number = 0;
    for (auto [dy, dx] : WINDOW) {
        number = number * 2 + (image[y + dy][x + dx] == '#'); 
    }
    return number;
}

/* Pad image using '.' in all directions */
Image pad_image(const Image &image, int padding) {
    Image result;
    std::string pad_line(image[0].size() + 2 * padding, '.');
    std::string pad_side(padding, '.');

    // Add top padding lines
    for (int i = 0; i < padding; ++i) {
        result.push_back(pad_line);
    }
    
    // Add padding to the sides
    for (const std::string &line : image) {
        result.push_back(pad_side + line + pad_side);
    }

    // Add bottom padding lines
    for (int i = 0; i < padding; ++i) {
        result.push_back(pad_line);
    }

    return result;
}

/* Enhance image [steps] number of times and return number of lit pixels */
int enhance_image(const Image &image, EnhanceAlgo algo, int steps) {
    // Final image will be at most [steps] number of pixels bigger
    // in all directions. We need to pad with 2 * [steps], because
    // we loose one pixel in every step because of the border.
    Image padded = pad_image(image, 2 * steps);

    for (int i = 0; i < steps; ++i) {
        Image new_image;

        // Get new pixels for image without its border
        for (decltype(padded.size()) y = 1; y < padded.size() - 1; ++y) {
            std::ostringstream new_line;
            for (decltype(padded[y].size()) x = 1; x < padded[y].size() - 1; ++x) {
                new_line << algo[get_window_number(padded, x, y)]; 
            }
            new_image.push_back(new_line.str());
        } 

        padded = new_image;
    }

    // Return number of turned on pixels
    return std::accumulate(padded.begin(), padded.end(), 0, [](int a, std::string b) {
        return a + std::count(b.begin(), b.end(), '#');
    });
}

std::pair<Image, EnhanceAlgo> read_file(const std::string &filename) {
    std::ifstream in;
    in.open(filename);

    std::ostringstream algo;
    std::string line;
    
    // Read lines until empty line
    while (std::getline(in, line) && line.size() > 1) {
        algo << line;
    }
    
    // Save each image line as a string
    Image image;

    while (std::getline(in, line)) {
        image.push_back(line);
    }

    return { image, algo.str() };
}

void test() {
    auto [image, algo] = read_file("inputs/input20_test.txt");
    assert(get_window_number(image, 2, 2) == 34);
    assert(enhance_image(image, algo, 2) == 35);
    assert(enhance_image(image, algo, 50) == 3351);
}


int main() {
    test();

    auto [image, algo] = read_file("inputs/input20.txt");
    std::cout << enhance_image(image, algo, 2) << '\n'; 
    std::cout << enhance_image(image, algo, 50) << '\n'; 
    return 0;
}
