#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct coord_t {
    long x;
    long y;

    bool operator==(const coord_t& coord) const {
        return x == coord.x && y == coord.y;
    }
};

struct coord_hash {
    size_t operator()(coord_t const& xy) const {
        std::hash<long> h{};
        return h(xy.x) ^ (h(xy.y) * 0x0123456789ABCDEF);
    }
};

template<class T>
class sparse_grid {
public:

    using map_type = std::unordered_map<coord_t, T, coord_hash>;

    sparse_grid() = default;

    [[nodiscard]] bool contains(long x, long y) const {
        return items.find({x,y}) != items.end();
    }

    T const& get(long x, long y, T const& def) const {
        auto it = items.find({x,y});
        if (it != items.end()) {
            return it->second;
        } else {
            return def;
        }
    }

    void set(long x, long y, T item) {
        items[{x,y}] = std::move(item);
    }

    T& ref(long x, long y) {
        return items[{x,y}];
    }

    size_t size() const {
        return items.size();
    }

    typename map_type::const_iterator begin() const {
        return items.begin();
    }

    typename map_type::const_iterator end() const {
        return items.end();
    }

private:
    map_type items;
};

sparse_grid<char> parse_input(std::istream& is) {
    sparse_grid<char> ret;
    std::string line;
    long y = 0;
    while (std::getline(is, line)) {
        for (long x = 0; x < static_cast<long>(line.size()); ++x) {
            if (line[x] == '#') {
                ret.set(x, y, 1);
            }
        }
        ++y;
    }
    return ret;
}

enum class direction {
    NORTH,
    SOUTH,
    WEST,
    EAST,
};

bool alone(long x, long y, sparse_grid<char> const& grid) {
    for (long x0 = x-1; x0 <= x+1; ++x0) {
        for (long y0 = y-1; y0 <= y+1; ++y0) {
            if (grid.contains(x0, y0) && !(x0 == x && y0 == y)) {
                return false;
            }
        }
    }
    return true;
}

void vote(long x, long y, std::vector<direction> const& dirs, sparse_grid<char> const& grid, sparse_grid<coord_t>& votes) {
    if (alone(x, y, grid)) {
        votes.set(x, y, {x,y});
        return;
    }
    for (auto dir : dirs) {
        if (dir == direction::NORTH && !grid.contains(x-1, y-1) && !grid.contains(x, y-1) && !grid.contains(x+1, y-1)) {
            votes.set(x, y, {x, y-1});
            return;
        } else if (dir == direction::SOUTH && !grid.contains(x-1, y+1) && !grid.contains(x, y+1) && !grid.contains(x+1, y+1)) {
            votes.set(x, y, {x, y+1});
            return;
        } else if (dir == direction::WEST && !grid.contains(x-1, y-1) && !grid.contains(x-1, y) && !grid.contains(x-1, y+1)) {
            votes.set(x, y, {x-1, y});
            return;
        } else if (dir == direction::EAST && !grid.contains(x+1, y-1) && !grid.contains(x+1, y) && !grid.contains(x+1, y+1)) {
            votes.set(x, y, {x+1, y});
            return;
        }
    }
    votes.set(x, y, {x,y});
}

bool evolve(sparse_grid<char>& grid, std::vector<direction> const& dirs) {
    sparse_grid<coord_t> votes;
    for (auto [xy, occupied] : grid) {
        vote(xy.x, xy.y, dirs, grid, votes);
    }

    sparse_grid<long> vote_counts;
    for (auto [src_xy, dst_xy] : votes) {
        vote_counts.ref(dst_xy.x, dst_xy.y)++;
    }

    bool anyone_moved = false;
    sparse_grid<char> result;
    for (auto [xy, occupied] : grid) {
        auto const dst = votes.ref(xy.x, xy.y);
        if (vote_counts.ref(dst.x, dst.y) == 1) {
            if (dst.x != xy.x || dst.y != xy.y) {
                anyone_moved = true;
            }
            result.set(dst.x, dst.y, 1);
        } else {
            result.set(xy.x, xy.y, 1);
        }
    }
    grid = std::move(result);
    return anyone_moved;
}

bool step(sparse_grid<char>& grid, std::vector<direction>& dirs) {
    auto output = evolve(grid, dirs);
    std::rotate(dirs.begin(), dirs.begin()+1, dirs.end());
    return output;
}

template<class T>
std::pair<coord_t, coord_t> bounds(sparse_grid<T> const& grid) {
    long const minx = std::min_element(grid.begin(), grid.end(), [](auto &p1, auto& p2) { return p1.first.x < p2.first.x; })->first.x;
    long const maxx = std::max_element(grid.begin(), grid.end(), [](auto &p1, auto& p2) { return p1.first.x < p2.first.x; })->first.x;
    long const miny = std::min_element(grid.begin(), grid.end(), [](auto &p1, auto& p2) { return p1.first.y < p2.first.y; })->first.y;
    long const maxy = std::max_element(grid.begin(), grid.end(), [](auto &p1, auto& p2) { return p1.first.y < p2.first.y; })->first.y;

    return {{minx,miny},{maxx,maxy}};
}

long count_open(sparse_grid<char> const& grid) {
    auto [min,max] = bounds(grid);
    return (max.x - min.x + 1) * (max.y - min.y + 1) - static_cast<long>(grid.size());
}

int main() {
    auto initial = parse_input(std::cin);

    std::vector<direction> dirs{direction::NORTH, direction::SOUTH, direction::WEST, direction::EAST};
    auto grid = initial;
    for (size_t i = 0; i < 10; ++i) {
        step(grid, dirs);
    }
    std::cout << count_open(grid) << "\n";

    size_t iter = 10;
    do {
        ++iter;
    } while(step(grid, dirs));
    std::cout << iter << "\n";
}
