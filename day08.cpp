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

template<std::forward_iterator TreesIter, std::forward_iterator VisibleIter>
requires std::is_convertible_v<std::iter_value_t<TreesIter>, long> &&
         std::is_convertible_v<std::iter_reference_t<VisibleIter>, visible_t&>
void mark_visible(TreesIter begin, TreesIter end, VisibleIter visible, bool (visible_t::*mem_ptr)) {
    long highest = -1;
    for (; begin != end; ++begin, ++visible) {
        if (*begin > highest) {
            (*visible).*mem_ptr = true;
        }
        highest = std::max(highest, *begin);
    }
}

grid<visible_t> mark_visible(grid<long> const& trees) {
    grid<visible_t> visible(trees.width(), trees.height());

    // From north/south
    for (long x = 0; x < trees.width(); ++x) {
        mark_visible(trees.col_begin(x), trees.col_end(x), visible.col_begin(x), &visible_t::visible_north);
        mark_visible(trees.col_rbegin(x), trees.col_rend(x), visible.col_rbegin(x), &visible_t::visible_south);
    }

    // From west/east
    for (long y = 0; y < trees.height(); ++y) {
        mark_visible(trees.row_begin(y), trees.row_end(y), visible.row_begin(y), &visible_t::visible_west);
        mark_visible(trees.row_rbegin(y), trees.row_rend(y), visible.row_rbegin(y), &visible_t::visible_east);
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
