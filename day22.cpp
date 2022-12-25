#include "grid.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

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

struct vector3 {
    long x = 0;
    long y = 0;
    long z = 0;
};

struct state_t {
    vector3 pos2d;
    vector3 pos3d;
    vector3 facing2d;
    vector3 facing3d;
};

class geometry {
public:
    virtual ~geometry() = default;
    [[nodiscard]] virtual state_t turn(state_t state, turn_t turn) const = 0;
    [[nodiscard]] virtual state_t forward(state_t state) const = 0;
};

state_t move(grid<char> const& grid, state_t state, op_t op, geometry const& geo) {
    state = geo.turn(state, op.turn);
    for (long i = 0; i < op.dist; ++i) {
        state_t const next = geo.forward(state);
        if (grid.at(next.pos2d.x, next.pos2d.y) == '.') {
            state = next;
        }
    }
    return state;
}

class geo2d_t: public geometry {
public:
    explicit geo2d_t(grid<char> const& map): map(map) {}

    static state_t rotate_left(state_t state) {
        // Rot matrix:
        // [ 0  1
        //  -1  0]
        return state_t{state.pos2d,
                       state.pos3d,
                       {state.facing2d.y, -state.facing2d.x, state.facing2d.z},
                       {state.facing3d.y, -state.facing3d.x, state.facing3d.z}};
    }

    [[nodiscard]] state_t turn(state_t state, turn_t turn) const override {
        // z-points into the floor to make a right-handed system.
        if (turn == turn_t::LEFT) {
            state = rotate_left(state);
        } else if (turn== turn_t::RIGHT) {
            state = rotate_left(rotate_left(rotate_left(state)));
        }
        return state;
    }

    [[nodiscard]] state_t forward(state_t state) const override {
        long new_x = state.pos2d.x + state.facing2d.x;
        long new_y = state.pos2d.y + state.facing2d.y;
        if (state.facing2d.x != 0 && (new_x < 0 || new_x >= map.width() || map.at(new_x, new_y) == ' ')) {
            if (state.facing2d.x > 0) {
                auto left_edge = std::find_if(map.row_begin(new_y), map.row_end(new_y), [](char c) { return c != ' '; });
                new_x = left_edge - map.row_begin(new_y);
            } else {
                auto right_edge = std::find_if(map.row_begin(new_y) + state.pos2d.x, map.row_end(new_y), [](char c) { return c == ' '; });
                new_x = (right_edge - map.row_begin(new_y)) - 1;
            }
        }
        if (state.facing2d.y != 0 && (new_y < 0 || new_y >= map.height() || map.at(new_x, new_y) == ' ')) {
            if (state.facing2d.y > 0) {
                auto top_edge = std::find_if(map.col_begin(new_x), map.col_end(new_x), [](char c) { return c != ' '; });
                new_y = top_edge - map.col_begin(new_x);
            } else {
                auto bottom_edge = std::find_if(map.col_begin(new_x) + state.pos2d.y, map.col_end(new_x), [](char c) { return c == ' '; });
                new_y = (bottom_edge - map.col_begin(new_x)) - 1;
            }
        }
        return state_t{{new_x, new_y, state.pos2d.z},
                       {new_x, new_y, state.pos2d.z},
                       state.facing2d,
                       state.facing3d};
    }

private:
    grid<char> const& map;
};

long score(vector3 const& facing2d) {
    if (facing2d.x > 0) {
        return 0;
    } else if (facing2d.x < 0) {
        return 2;
    } else if (facing2d.y > 0) {
        return 1;
    } else {
        return 3;
    }
}

long score(state_t const& state) {
    return 1000 * (state.pos2d.y + 1) + 4 * (state.pos2d.x + 1) + score(state.facing2d);
}

int main() {
    auto const [grid, path] = parse_input(std::cin);
    long const start_x = std::find_if(grid.row_begin(0), grid.row_end(0), [](char c) { return c != ' '; }) - grid.row_begin(0);
    state_t state{{start_x, 0, 0}, {start_x, 0, 0}, {1, 0, 0}, {1, 0, 0}};
    geo2d_t geo2d{grid};

    for (op_t const op : path) {
        state = move(grid, state, op, geo2d);
    }
    std::cout << score(state) << "\n";
}
