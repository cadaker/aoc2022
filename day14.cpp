#include "grid.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <limits>
#include <algorithm>

struct coord {
    long x = 0;
    long y = 0;
};

using trace = std::vector<coord>;

std::vector<trace> parse_input(std::istream& is) {
    std::regex const point_pattern(R"(([0-9]+),([0-9]+))");
    std::vector<trace> ret;
    std::string line;
    while (std::getline(is, line)) {
        trace tr;
        std::sregex_iterator it{line.begin(), line.end(), point_pattern};
        std::sregex_iterator end{};
        for (; it != end; ++it) {
            std::smatch const &m = *it;
            long const x = std::stol(m[1]);
            long const y = std::stol(m[2]);
            tr.push_back({x, y});
        }
        ret.push_back(std::move(tr));
    }
    return ret;
}

std::pair<coord, coord> bounds(std::vector<trace> const& traces) {
    long minx = std::numeric_limits<long>::max();
    long maxx = std::numeric_limits<long>::min();
    long miny = std::numeric_limits<long>::max();
    long maxy = std::numeric_limits<long>::min();

    for (auto const& tr : traces) {
        if (!tr.empty()) {
            auto [mnx,mxx] = std::minmax_element(tr.begin(), tr.end(), [](coord const& c1, coord const& c2) {
                return c1.x < c2.x;
            });
            auto [mny,mxy] = std::minmax_element(tr.begin(), tr.end(), [](coord const& c1, coord const& c2) {
                return c1.y < c2.y;
            });
            minx = std::min(minx, mnx->x);
            miny = std::min(miny, mny->y);
            maxx = std::max(maxx, mxx->x);
            maxy = std::max(maxy, mxy->y);
        }
    }
    return {{minx, miny}, {maxx, maxy}};
}

struct offset_map {
    coord offset{};
    grid<char> map;
};

offset_map make_map(std::vector<trace> const& traces) {
    auto [mn, mx] = bounds(traces);
    // Include the sand source (500, 0) in the bounds.
    mn.y = std::min(mn.y, 0L);
    mx.y = std::max(mx.y, 0L);
    mn.x = std::min(mn.x, 500L);
    mx.x = std::max(mx.x, 500L);

    // Include a couple of extra, to support the optional floor.
    long const height = mx.y - mn.y + 6;
    // Support a massive triangle all the way from the top.
    mn.x = std::min(mn.x, 500 - height - 1);
    mx.x = std::max(mx.x, 500 + height + 1);
    long const width = mx.x - mn.x + 3;

    return {{mn.x-1, mn.y-1}, {static_cast<size_t>(width), static_cast<size_t>(height)}};
}

void paint_rocks(offset_map &map, trace const& tr) {
    if (tr.empty()) {
        return;
    }
    long x = tr[0].x;
    long y = tr[0].y;
    for (size_t i = 1; i < tr.size(); ++i) {
        coord const pos = tr[i];
        if (x == pos.x) {
            long const dy = y < pos.y ? 1 : -1;
            for (; y != pos.y; y += dy) {
                map.map.at(x - map.offset.x, y - map.offset.y) = '#';
            }
            map.map.at(x - map.offset.x, y - map.offset.y) = '#';
        } else {
            long const dx = x < pos.x ? 1 : -1;
            for (; x != pos.x; x += dx) {
                map.map.at(x - map.offset.x, y - map.offset.y) = '#';
            }
            map.map.at(x - map.offset.x, y - map.offset.y) = '#';
        }
    }
}

offset_map create_map(std::vector<trace> const& traces, bool floor) {
    auto map = make_map(traces);
    for (auto const& tr : traces) {
        paint_rocks(map, tr);
    }
    if (floor) {
        auto [mn, mx] = bounds(traces);
        long const floor_y = mx.y + 2;
        paint_rocks(map, {{map.offset.x, floor_y},
                          {map.offset.x + map.map.width() - 1, floor_y}});
    }
    return map;
}

bool move(grid<char> const& map, long& x, long& y) {
    for (coord next : {coord{x, y+1}, {x-1, y+1}, {x+1, y+1}}) {
        if (map.at(next.x, next.y) == 0) {
            x = next.x;
            y = next.y;
            return true;
        }
    }
    return false;
}

bool drop_sand_grain(offset_map& map, coord pos) {
    long x = pos.x - map.offset.x;
    long y = pos.y - map.offset.y;

    while (true) {
        if (y+1 >= map.map.height()) {
            return false;
        }

        if (!move(map.map, x, y)) {
            map.map.at(x, y) = 'o';
            return true;
        }
    }
}

size_t drop_until_done(offset_map& map, coord sand_pos) {
    size_t count = 0;
    while (drop_sand_grain(map, sand_pos)) {
        ++count;
    }
    return count;
}

size_t drop_until_blocked(offset_map& map, coord sand_pos) {
    size_t count = 0;
    while (map.map.at(sand_pos.x - map.offset.x, sand_pos.y - map.offset.y) == 0) {
        drop_sand_grain(map, sand_pos);
        ++count;
    }
    return count;
}

int main() {
    auto traces = parse_input(std::cin);
    auto map = create_map(traces, false);

    std::cout << drop_until_done(map, {500, 0}) << "\n";

    map = create_map(traces, true);
    std::cout << drop_until_blocked(map, {500, 0}) << "\n";
}
