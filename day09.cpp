#include <iostream>
#include <vector>
#include <unordered_set>

enum class dir_t {
    right,
    up,
    left,
    down,
};

struct op_t {
    dir_t dir;
    long steps;
};

dir_t parse_dir(char c) {
    if (c == 'U') {
        return dir_t::up;
    } else if (c == 'D') {
        return dir_t::down;
    } else if (c == 'R') {
        return dir_t::right;
    } else {
        return dir_t::left;
    }
}

std::vector<op_t> parse_input(std::istream& is) {
    std::vector<op_t> ops;
    std::string line;
    while (std::getline(is, line)) {
        if (line.size() < 3) {
            std::cerr << "Invalid line: " << line << std::endl;
        } else {
            ops.push_back({parse_dir(line[0]), std::stol(line.substr(2))});
        }
    }
    return ops;
}

struct coord_t {
    long x = 0;
    long y = 0;
};

bool operator==(coord_t p1, coord_t p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(coord_t p1, coord_t p2) {
    return !(p1 == p2);
}

struct coord_hash {
    size_t operator()(coord_t const& p) const {
        std::hash<long> h{};
        return h(p.x ^ p.y);
    }
};

struct rope_t {
    // First one is the head.
    std::vector<coord_t> knots;
};

coord_t step(coord_t pos, dir_t dir) {
    long dx = 0;
    long dy = 0;
    switch (dir) {
        case dir_t::right: dx = 1; break;
        case dir_t::up: dy = 1; break;
        case dir_t::left: dx = -1; break;
        case dir_t::down: dy = -1; break;
    }
    return {pos.x + dx, pos.y + dy};
}

bool adjacent(coord_t pos1, coord_t pos2) {
    long const dx = pos1.x - pos2.x;
    long const dy = pos1.y - pos2.y;
    return -1 <= dx && dx <= 1 && -1 <= dy && dy <= 1;
}

long move_tail_coord(long head, long tail) {
    if (head > tail) {
        return tail+1;
    } else if (head < tail) {
        return tail-1;
    } else {
        return tail;
    }
}

coord_t move_tail(coord_t head_pos, coord_t tail_pos) {
    return {move_tail_coord(head_pos.x, tail_pos.x), move_tail_coord(head_pos.y, tail_pos.y)};
}

coord_t follow(coord_t head_pos, coord_t tail_pos) {
    if (!adjacent(head_pos, tail_pos)) {
        return move_tail(head_pos, tail_pos);
    } else {
        return tail_pos;
    }
}

auto move(std::vector<op_t> const& ops, size_t rope_len) {
    rope_t rope;
    rope.knots.resize(rope_len);
    std::unordered_set<coord_t, coord_hash> tail_positions = {{0,0}};

    if (rope_len == 0) {
        return tail_positions;
    }

    for (op_t op : ops) {
        for (long s = 0; s < op.steps; ++s) {
            rope.knots.front() = step(rope.knots.front(), op.dir);
            for (size_t i = 1; i < rope.knots.size(); ++i) {
                rope.knots[i] = follow(rope.knots[i-1], rope.knots[i]);
            }
            tail_positions.insert(rope.knots.back());
        }
    }
    return tail_positions;
}

int main() {
    auto const input = parse_input(std::cin);

    std::cout << move(input, 2).size() << "\n";
    std::cout << move(input, 10).size() << "\n";
}
