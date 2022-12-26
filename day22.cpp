#include "grid.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

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

vector3 operator-(vector3 const& v) {
    return {-v.x, -v.y, -v.z};
}

vector3 operator-(vector3 const& u, vector3 const& v) {
    return {u.x - v.x, u.y - v.y, u.z - v.z};
}

vector3 operator+(vector3 const& u, vector3 const& v) {
    return {u.x + v.x, u.y + v.y, u.z + v.z};
}

vector3 operator*(long scale, vector3 const& u) {
    return {scale * u.x, scale * u.y, scale * u.z};
}

bool operator==(vector3 const& u, vector3 const& v) {
    return u.x == v.x && u.y == v.y && u.z == v.z;
}

std::ostream& operator<<(std::ostream& os, vector3 const& v) {
    return os << v.x << "," << v.y << "," << v.z;
}

vector3 cross(vector3 const& u, vector3 const& v) {
    return {
            u.y*v.z - u.z*v.y,
            u.z*v.x - u.x*v.z,
            u.x*v.y - u.y*v.x,
    };
}

long inner(vector3 const& u, vector3 const& v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

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

long compute_square_side(grid<char> const& grid) {
    long const points = std::count_if(grid.begin(), grid.end(), [](char c) { return c != ' '; });
    long const points_per_side = points / 6;
    long const side = static_cast<long>(round(sqrt(static_cast<double>(points_per_side))));
    if (side*side*6 != points) {
        throw std::runtime_error("map doesn't fit on a cube");
    }
    return side;
}

struct side_t {
    vector3 normal;
    vector3 down;
    vector3 right;
    // top left in the map
    vector3 start2d;
    // bottom right in the map
    vector3 end2d;
    // 3d pos of top right point
    vector3 start3d;
};

side_t const* find_side_by_pos2d(std::vector<side_t> const& sides, long x, long y) {
    for (side_t const& side : sides) {
        if (side.start2d.x <= x && x < side.end2d.x &&
            side.start2d.y <= y && y < side.end2d.y) {
            return &side;
        }
    }
    return nullptr;
}

side_t const* find_side_by_normal(std::vector<side_t> const& sides, vector3 normal) {
    for (side_t const& side : sides) {
        if (side.normal.x == normal.x && side.normal.y == normal.y && side.normal.z == normal.z) {
            return &side;
        }
    }
    return nullptr;
}

std::vector<side_t> find_sides(grid<char> const& map, long side) {
    std::vector<side_t> sides;
    sides.reserve(6);

    long const start_x = std::find_if(map.row_begin(0), map.row_end(0), [](char c) { return c != ' '; }) - map.row_begin(0);
    sides.push_back({{0, 0, -1},
                     {0, 1, 0},
                     {1, 0, 0},
                     {start_x, 0, 0},
                     {start_x+side, 0+side, 0},
                     {0, 0, 0}});

    std::vector<size_t> stack = {0};
    while (!stack.empty()) {
        size_t const side_index = stack.back();
        stack.pop_back();
        side_t current_side = sides[side_index];

        long const x = current_side.start2d.x;
        long const y = current_side.start2d.y;
        // Look left
        if (x - side >= 0 && map.at(x-side, y) != ' ' && find_side_by_pos2d(sides, x-side, y) == nullptr) {
            side_t new_side{
                    -current_side.right, // normal
                    current_side.down, // down
                    current_side.normal, // right
                    {current_side.start2d.x - side, current_side.start2d.y, 0},
                    {current_side.end2d.x - side, current_side.end2d.y, 0},
                    current_side.start3d - (side-1)*current_side.normal,
            };
            sides.push_back(new_side);
            stack.push_back(sides.size()-1);
        }
        // Look right
        if (x + side < map.width() && map.at(x + side, y) != ' ' && find_side_by_pos2d(sides, x+side, y) == nullptr) {
            side_t new_side{
                    current_side.right, // normal
                    current_side.down, // down
                    -current_side.normal, // right
                    {current_side.start2d.x + side, current_side.start2d.y, 0},
                    {current_side.end2d.x + side, current_side.end2d.y, 0},
                    current_side.start3d + (side-1)*current_side.right,
            };
            sides.push_back(new_side);
            stack.push_back(sides.size()-1);
        }
        // Look down
        if (y + side < map.height() && map.at(x, y + side) != ' ' && find_side_by_pos2d(sides, x, y+side) == nullptr) {
            side_t new_side{
                    current_side.down, // normal
                    -current_side.normal, // down
                    current_side.right, // right
                    {current_side.start2d.x, current_side.start2d.y + side, 0},
                    {current_side.end2d.x, current_side.end2d.y + side, 0},
                    current_side.start3d + (side-1)*current_side.down,
            };
            sides.push_back(new_side);
            stack.push_back(sides.size()-1);
        }
        // Look up
        if (y - side >= 0 && map.at(x, y - side) != ' ' && find_side_by_pos2d(sides, x, y-side) == nullptr) {
            side_t new_side{
                    -current_side.down, // normal
                    current_side.normal, // down
                    current_side.right, // right
                    {current_side.start2d.x, current_side.start2d.y - side, 0},
                    {current_side.end2d.x, current_side.end2d.y - side, 0},
                    current_side.start3d - (side-1)*current_side.normal,
            };
            sides.push_back(new_side);
            stack.push_back(sides.size()-1);
        }
    }
    return sides;
}

class geo3d_t: public geometry {
public:
    explicit geo3d_t(std::vector<side_t> sides)
            : sides(std::move(sides))
    {}

    static vector3 turn_left(vector3 facing, vector3 normal) {
        return cross(normal, facing);
    }

    static vector3 facing2d(vector3 facing3d, side_t const& side) {
        if (facing3d == side.right) {
            return {1, 0, 0};
        } else if (facing3d == -side.right) {
            return {-1, 0, 0};
        } else if (facing3d == side.down) {
            return {0, 1, 0};
        } else {
            return {0, -1, 0};
        }
    }

    [[nodiscard]] state_t turn(state_t state, turn_t turn) const override {
        auto const* side = find_side_by_pos2d(sides, state.pos2d.x, state.pos2d.y);
        if (turn == turn_t::LEFT) {
            state.facing3d = cross(side->normal, state.facing3d);
        } else if(turn == turn_t::RIGHT) {
            state.facing3d = -cross(side->normal, state.facing3d);
        }
        state.facing2d = facing2d(state.facing3d, *side);
        return state;
    }

    [[nodiscard]] state_t forward(state_t state) const override {
        long const x = state.pos2d.x;
        long const y = state.pos2d.y;
        auto const* side = find_side_by_pos2d(sides, x, y);

        long const new_x = x + state.facing2d.x;
        long const new_y = y + state.facing2d.y;

        side_t const* new_side = nullptr;
        if (new_x < side->start2d.x) {
            new_side = find_side_by_normal(sides, -side->right);
        } else if (new_x >= side->end2d.x) {
            new_side = find_side_by_normal(sides, side->right);
        } else if (new_y < side->start2d.y) {
            new_side = find_side_by_normal(sides, -side->down);
        } else if (new_y >= side->end2d.y) {
            new_side = find_side_by_normal(sides, side->down);
        }
        if (new_side == nullptr) {
            return {{new_x, new_y, 0},
                    {state.pos3d.x + state.facing3d.x, state.pos3d.y + state.facing3d.y, state.pos3d.z + state.facing3d.z},
                    state.facing2d,
                    state.facing3d};
        } else {
            state.facing3d = -side->normal;
            state.facing2d = facing2d(state.facing3d, *new_side);
            state.pos2d = map_3d_to_2d(state.pos3d, *new_side);
            return state;
        }
    }

    static vector3 map_3d_to_2d(vector3 pos3d, side_t const& side) {
        long right_coord = inner(side.right, pos3d - side.start3d);
        long down_coord = inner(side.down, pos3d - side.start3d);
        return {side.start2d.x + right_coord, side.start2d.y + down_coord, 0};
    }

private:
    std::vector<side_t> sides;
};

int main() {
    auto const [grid, path] = parse_input(std::cin);
    long const start_x = std::find_if(grid.row_begin(0), grid.row_end(0), [](char c) { return c != ' '; }) - grid.row_begin(0);
    state_t state{{start_x, 0, 0}, {start_x, 0, 0}, {1, 0, 0}, {1, 0, 0}};
    geo2d_t geo2d{grid};

    for (op_t const op : path) {
        state = move(grid, state, op, geo2d);
    }
    std::cout << score(state) << "\n";

    long const square_side = compute_square_side(grid);
    auto const sides = find_sides(grid, square_side);
    geo3d_t geo3d{sides};
    state_t state3d{{start_x, 0, 0}, {0, 0, 0}, {1, 0, 0}, {1, 0, 0}};

    for (op_t const op : path) {
        state3d = move(grid, state3d, op, geo3d);
    }
    std::cout << score(state3d) << "\n";
}
