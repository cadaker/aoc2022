#include "grid.hpp"
#include <iostream>
#include <string>

grid<long> parse_input(std::istream& is) {
    grid_builder<long> builder;
    std::string line;
    while (std::getline(is, line)) {
        for (char c : line) {
            builder.push_back(c - '0');
        }
        builder.end_line();
    }
    return std::move(builder).finish();
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

template<class TreesIter>
requires std::is_convertible_v<std::iter_value_t<TreesIter>, long>
long scan(TreesIter begin, TreesIter end) {
    if (begin == end) {
        return 0;
    }
    long count = 0;
    for (TreesIter it = begin+1; it != end; ++it) {
        ++count;
        if (*it >= *begin) {
            break;
        }
    }
    return count;
}

long score_at(grid<long> const& trees, long x, long y) {
    long const east = scan(trees.row_iter_at(x, y), trees.row_end(y));
    long const west = scan(trees.row_riter_at(x, y), trees.row_rend(y));
    long const south = scan(trees.col_iter_at(x, y), trees.col_end(x));
    long const north = scan(trees.col_riter_at(x, y), trees.col_rend(x));
    return east * west * north * south;
}

grid<long> scenic_scores(grid<long> const& trees) {
    grid<long> scores(trees.width(), trees.height());

    for (long x = 0; x < trees.width(); ++x) {
        for (long y = 0; y < trees.height(); ++y) {
            scores.at(x, y) = score_at(trees, x, y);
        }
    }
    return scores;
}

int main() {
    auto trees = parse_input(std::cin);

    auto visible = mark_visible(trees);

    std::cout << std::count_if(visible.begin(), visible.end(), [](visible_t const& v) {
        return v.visible();
    }) << "\n";
    auto scores = scenic_scores(trees);
    auto it = std::max_element(scores.begin(), scores.end());
    std::cout << (it == trees.end() ? -1 : *it) << "\n";
}
