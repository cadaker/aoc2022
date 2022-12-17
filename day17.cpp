#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

std::vector<char> parse_input(std::istream& is) {
    std::string line;
    std::getline(is, line);
    return {line.begin(), line.end()};
}

struct coord_t {
    long x = 0;
    long y = 0;
};

using block_t = std::vector<coord_t>;

std::vector<block_t> const blocks = {
        // ####
        {{0,0}, {1,0}, {2,0},{3,0}},
        // .#.
        // ###
        // .#.
        {{0,1}, {1,0}, {1,1}, {1,2}, {2,1}},
        // ..#
        // ..#
        // ###
        {{0,0}, {1,0}, {2,0}, {2,1}, {2,2}},
        // #
        // #
        // #
        // #
        {{0,0}, {0,1}, {0,2}, {0,3}},
        // ##
        // ##
        {{0,0}, {0,1}, {1,0}, {1,1}},
};

template<class T>
class cycle {
public:
    explicit cycle(std::vector<T> const& v)
            : v(v)
    {}

    T const& next() {
        auto& ret = v.at(i++);
        i %= v.size();
        return ret;
    }

    [[nodiscard]] size_t index() const {
        return i;
    }
private:
    std::vector<T> const& v;
    size_t i = 0;
};

using sparse_grid_t = std::unordered_map<long, std::unordered_map<long, char>>;

char lookup(sparse_grid_t const& grid, coord_t const& xy) {
    auto it_x = grid.find(xy.x);
    if (it_x != grid.end()) {
        auto it_y = it_x->second.find(xy.y);
        if (it_y != it_x->second.end()) {
            return it_y->second;
        }
    }
    return 0;
}

constexpr long WIDTH = 7;

struct board_t {
    sparse_grid_t grid;
    long top_y = 0;
};

void translate(block_t& block, long dx, long dy) {
    for (coord_t& xy : block) {
        xy.x += dx;
        xy.y += dy;
    }
}

bool can_move_sideways(sparse_grid_t const& grid, block_t const& block, long dx) {
    for (coord_t const& xy : block) {
        coord_t const next{xy.x + dx, xy.y};
        if (next.x < 0 || next.x >= WIDTH || lookup(grid, next) == '#') {
            return false;
        }
    }
    return true;
}

bool can_move_downwards(sparse_grid_t const& grid, block_t const& block) {
    for (coord_t const& xy : block) {
        coord_t const below{xy.x, xy.y-1};
        if (below.y < 0 || lookup(grid, below) == '#') {
            return false;
        }
    }
    return true;
}

void fix_block(board_t& board, block_t const& block) {
    for (coord_t const& xy : block) {
        board.grid[xy.x][xy.y] = '#';
        board.top_y = std::max(board.top_y, xy.y+1);
    }
}

coord_t drop_block(board_t& board, block_t block, cycle<char>& jets) {
    translate(block, 2, board.top_y + 3);
    while (true) {
        long const dx = jets.next() == '>' ? 1 : -1;
        if (can_move_sideways(board.grid, block, dx)) {
            translate(block, dx, 0);
        }
        if (can_move_downwards(board.grid, block)) {
            translate(block, 0, -1);
        } else {
            fix_block(board, block);
            return block.front();
        }
    }
}

void run_board(board_t& board, std::vector<char> const& input, size_t n) {
    cycle<char> jets(input);
    cycle<block_t> drops(blocks);
    for (size_t i = 0; i < n; ++i) {
        drop_block(board, drops.next(), jets);
    }
}

template<class T>
std::pair<long, long> find_cycle(T t0, std::function<void(T&)> const& f) {
    auto t1 = t0;
    auto t2 = t0;

    while (true) {
        f(t1);
        f(t2);
        f(t2);
        if (t1 == t2) {
            break;
        }
    }
    long offset = 0;
    auto t3 = t0;
    while (t1 != t3) {
        f(t1);
        f(t3);
        ++offset;
    }

    long length = 1;
    f(t1);
    while (t1 != t3) {
        f(t1);
        ++length;
    }
    return {offset, length};
}

struct state_t {
    board_t board;
    cycle<block_t> drops;
    cycle<char> jets;
};

coord_t step(state_t& state) {
    return drop_block(state.board, state.drops.next(), state.jets);
}

void step_void(state_t& state) {
    step(state);
}

bool operator==(state_t const& s1, state_t const& s2) {
    if (s1.drops.index() != s2.drops.index()) {
        return false;
    }
    if (s1.jets.index() != s2.jets.index()) {
        return false;
    }
    // Try stepping a couple of steps forward, and see if they match.
    state_t tmp1 = s1;
    state_t tmp2 = s2;
    for (size_t i = 0; i < 20; ++i) {
        auto [x1,y1] = step(tmp1);
        auto [x2,y2] = step(tmp2);
        if (x1 != x2 || y1 - s1.board.top_y != y2 - s2.board.top_y) {
            return false;
        }
    }
    return true;
}

bool operator!=(state_t const& s1, state_t const& s2) {
    return !(s1 == s2);
}

int main() {
    auto const input = parse_input(std::cin);
    board_t board{};
    run_board(board, input, 2022);
    std::cout << board.top_y << "\n";

    auto [cycle_offset, cycle_length] = find_cycle<state_t>(state_t{{}, cycle{blocks}, cycle{input}}, step_void);

    long const N = 1'000'000'000'000L;

    state_t state{{}, cycle{blocks}, cycle{input}};
    for (long i = 0; i < cycle_offset; ++i) {
        step(state);
    }
    long const y0 = state.board.top_y;
    for (long i = 0; i < cycle_length; ++i) {
        step(state);
    }
    long const dy = state.board.top_y - y0;
    for (long i = 0; i < (N - cycle_length - cycle_offset) % cycle_length; ++i) {
        step(state);
    }
    long const loops = (N - cycle_length - cycle_offset) / cycle_length;
    std::cout << state.board.top_y + dy * loops << "\n";
}
