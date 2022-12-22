#include "grid.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

enum class dir_t {
    LEFT,
    UP,
    RIGHT,
    DOWN,
};

enum class turn_t {
    NONE,
    LEFT,
    RIGHT,
};

struct op_t {
    long dist;
    turn_t turn;
};

std::pair<grid<char>, std::vector<op_t>> parse_input(std::istream& is) {
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(is, line) && !line.empty()) {
        lines.push_back(line);
    }

    if (lines.empty()) {
        throw std::runtime_error("no input lines");
    }

    auto it = std::max_element(lines.begin(), lines.end(), [](auto const& ln1, auto const& ln2) {
        return ln1.size() < ln2.size();
    });
    size_t const width = it->size();

    grid_builder<char> builder;
    for (auto const& ln : lines) {
        for (char c : ln) {
            builder.push_back(c);
        }
        for (size_t i = ln.size(); i < width; ++i) {
            builder.push_back(' ');
        }
        builder.end_line();
    }

    std::vector<op_t> ops;
    std::getline(is, line);
    size_t i = 0;
    while (i < line.size()) {
        if (line[i] == 'L') {
            ops.push_back(op_t{0, turn_t::LEFT});
            ++i;
        } else if (line[i] == 'R') {
            ops.push_back(op_t{0, turn_t::RIGHT});
            ++i;
        } else {
            long dist = 0;
            while (isdigit(line[i])) {
                dist = 10 * dist + (line[i++] - '0');
            }
            ops.push_back(op_t{dist, turn_t::NONE});
        }
    }
    return {std::move(builder).finish(), std::move(ops)};
}

dir_t turn(dir_t dir, turn_t turn) {
    if (turn == turn_t::LEFT) {
        switch (dir) {
            case dir_t::RIGHT: return dir_t::UP;
            case dir_t::UP: return dir_t::LEFT;
            case dir_t::LEFT: return dir_t::DOWN;
            case dir_t::DOWN: return dir_t::RIGHT;
        }
    } else if (turn == turn_t::RIGHT) {
        switch (dir) {
            case dir_t::RIGHT: return dir_t::DOWN;
            case dir_t::UP: return dir_t::RIGHT;
            case dir_t::LEFT: return dir_t::UP;
            case dir_t::DOWN: return dir_t::LEFT;
        }
    }
    return dir;
}

std::pair<long, long> step(dir_t dir) {
    switch (dir) {
        case dir_t::LEFT: return {-1, 0};
        case dir_t::UP: return {0, -1};
        case dir_t::RIGHT: return {1, 0};
        case dir_t::DOWN: return {0, 1};
    }
    return {0,0};
}

struct pos_t {
    long x = 0;
    long y = 0;
    dir_t dir = dir_t::RIGHT;
};

struct map_t {
    grid<char> map;
    std::vector<long> left_edge;
    std::vector<long> right_edge;
    std::vector<long> top_edge;
    std::vector<long> bottom_edge;
};

map_t make_map(grid<char> grid) {
    std::vector<long> left_edge;
    std::vector<long> right_edge;
    for (long y = 0; y < grid.height(); ++y) {
        auto left = std::find_if(grid.row_begin(y), grid.row_end(y), [](char c) { return c != ' '; });
        auto right = std::find_if(left, grid.row_end(y), [](char c) { return c == ' '; });
        left_edge.push_back(left - grid.row_begin(y));
        right_edge.push_back(right - grid.row_begin(y));
    }
    std::vector<long> top_edge;
    std::vector<long> bottom_edge;
    for (long x = 0; x < grid.width(); ++x) {
        auto top = std::find_if(grid.col_begin(x), grid.col_end(x), [](char c) { return c != ' '; });
        auto bottom = std::find_if(top, grid.col_end(x), [](char c) { return c == ' '; });
        top_edge.push_back(top - grid.col_begin(x));
        bottom_edge.push_back(bottom - grid.col_begin(x));
    }
    return {std::move(grid), std::move(left_edge), std::move(right_edge), std::move(top_edge), std::move(bottom_edge)};
}

pos_t move(map_t const& map, pos_t pos, op_t op) {
    pos.dir = turn(pos.dir, op.turn);
    auto const [dx, dy] = step(pos.dir);
    for (long i = 0; i < op.dist; ++i) {
        long x = pos.x + dx;
        long y = pos.y + dy;
        if (dy == 0 && x < map.left_edge.at(y)) {
            x = map.right_edge.at(y) - 1;
        }
        if (dy == 0 && x >= map.right_edge.at(y)) {
            x = map.left_edge.at(y);
        }
        if (dx == 0 && y < map.top_edge.at(x)) {
            y = map.bottom_edge.at(x) - 1;
        }
        if (dx == 0 && y >= map.bottom_edge.at(x)) {
            y = map.top_edge.at(x);
        }
        if (map.map.at(x, y) == '.') {
            pos.x = x;
            pos.y = y;
        }
    }
    return pos;
}

long score(dir_t dir) {
    switch (dir) {
        case dir_t::RIGHT: return 0;
        case dir_t::UP: return 3;
        case dir_t::LEFT: return 2;
        case dir_t::DOWN: return 1;
        default: return 0;
    }
}

int main() {
    auto const [grid, path] = parse_input(std::cin);
    map_t const map = make_map(grid);

    pos_t pos;
    pos.x = map.left_edge.at(0);

    for (op_t const op : path) {
        pos = move(map, pos, op);
    }
    std::cout << (1000 * (pos.y + 1) + 4 * (pos.x + 1) + score(pos.dir)) << "\n";
}
