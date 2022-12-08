#include "grid.hpp"
#include <iostream>
#include <string>

grid<long> parse_input(std::istream& is) {
    size_t width = 0;
    std::vector<long> data;
    std::string line;
    while (std::getline(is, line)) {
        width = line.size();
        for (char c : line) {
            data.push_back(c - '0');
        }
    }
    return {width, data.begin(), data.end()};
}

struct visible_t {
    bool visible_north = false;
    bool visible_south = false;
    bool visible_east = false;
    bool visible_west = false;

    [[nodiscard]] bool visible() const {
        return visible_north || visible_south || visible_east || visible_west;
    }
};

grid<visible_t> mark_visible(grid<long> const& trees) {
    grid<visible_t> visible(trees.width(), trees.height());

    // From north
    for (long x = 0; x < trees.width(); ++x) {
        long highest = -1;
        for (long y = 0; y < trees.height(); ++y) {
            if (trees.at(x,y) > highest) {
                visible.at(x,y).visible_north = true;
            }
            highest = std::max(highest, trees.at(x,y));
        }
    }

    // From south
    for (long x = 0; x < trees.width(); ++x) {
        long highest = -1;
        for (long y = trees.height()-1; y >= 0; --y) {
            if (trees.at(x,y) > highest) {
                visible.at(x,y).visible_south = true;
            }
            highest = std::max(highest, trees.at(x,y));
        }
    }

    // From west
    for (long y = 0; y < trees.height(); ++y) {
        long highest = -1;
        for (long x = 0; x < trees.width(); ++x) {
            if (trees.at(x,y) > highest) {
                visible.at(x,y).visible_west = true;
            }
            highest = std::max(highest, trees.at(x,y));
        }
    }

    // From east
    for (long y = 0; y < trees.height(); ++y) {
        long highest = -1;
        for (long x = trees.width()-1; x >= 0; --x) {
            if (trees.at(x,y) > highest) {
                visible.at(x,y).visible_east = true;
            }
            highest = std::max(highest, trees.at(x,y));
        }
    }

    return visible;
}

int main() {
    auto trees = parse_input(std::cin);

    auto visible = mark_visible(trees);

    std::cout << std::count_if(visible.begin(), visible.end(), [](visible_t const& v) {
        return v.visible();
    }) << "\n";
}
