#include "grid.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>
#include <tuple>

struct coord {
    long x;
    long y;
};

enum class direction {
    RIGHT,
    UP,
    LEFT,
    DOWN,
};

struct blizzard {
    coord start;
    direction dir;
};

struct input {
    long width;
    long height;
    std::vector<blizzard> blizzards;
};

input parse_input(std::istream& is) {
    std::string line;
    if (!std::getline(is, line) || line.size() < 3) {
        throw std::runtime_error("invalid line");
    }
    long const width = static_cast<long>(line.size() - 2);
    std::vector<blizzard> blizzards;
    long y = 0;
    while (getline(is, line)) {
        for (long i = 1; i+1 < static_cast<long>(line.size()); ++i) {
            if (line[i] == '>') {
                blizzards.push_back({{i-1, y}, direction::RIGHT});
            } else if (line[i] == '<') {
                blizzards.push_back({{i-1, y}, direction::LEFT});
            } else if (line[i] == 'v') {
                blizzards.push_back({{i-1, y}, direction::DOWN});
            } else if (line[i] == '^') {
                blizzards.push_back({{i-1, y}, direction::UP});
            }
        }
        ++y;
    }
    return {width, y-1, std::move(blizzards)};
}

long gcd(long x, long y) {
    while (x > 0) {
        long tmp = y % x;
        y = x;
        x = tmp;
    }
    return y;
}

long modulo(long x, long n) {
    return ((x % n) + n) % n;
}

std::pair<long, long> dir_vector(direction dir) {
    switch (dir) {
        case direction::RIGHT: return {1, 0};
        case direction::UP: return {0, -1};
        case direction::LEFT: return {-1, 0};
        case direction::DOWN: return {0, 1};
    }
    return {0,0};
}

// We can view the entire problem as a 3d room, width x height x lcm(width, height),
// since the blizzards repeat their pattern after lcm(width, height) steps.

std::vector<grid<char>> build_3d_map(long width, long height, std::vector<blizzard> const& blizzards) {
    long const length = width * height / gcd(width, height);
    std::vector<grid<char>> map;

    for (long time = 0; time < length; ++time) {
        grid<char> level(width, height);

        for (blizzard const& b : blizzards) {
            auto const [dx, dy] = dir_vector(b.dir);
            long const x = modulo(b.start.x + time * dx, width);
            long const y = modulo(b.start.y + time * dy, height);
            level.at(x, y) = 1;
        }
        map.push_back(std::move(level));
    }
    return map;
}

long search(std::vector<grid<char>> const& map, long init_time, coord start, coord end) {
    long const length = static_cast<long>(map.size());
    long const width = map[0].width();
    long const height = map[0].height();

    struct state {
        long x;
        long y;
        long t;
    };

    // We may initially have to stand and wait until we can enter the start square...
    long start_time = init_time + 1;
    while (map[modulo(start_time, length)].at(start.x, start.y) != 0) {
        ++start_time;
    }

    std::deque<state> queue = {{state{start.x, start.y, start_time}}};
    std::vector<grid<char>> visited(map.size(), grid<char>(width, height));
    visited[modulo(start_time, length)].at(start.x, start.y) = 1;

    while (!queue.empty()) {
        auto const [x,y,t] = queue.front();
        queue.pop_front();

        if (x == end.x && y == end.y) {
            return t;
        }
        long const next_level = modulo(t+1, length);
        for (auto [dx, dy] : {std::pair<long,long>{-1,0}, {1,0}, {0,-1}, {0,1}, {0,0}}) {
            if (0 <= x+dx && x+dx < width &&
                0 <= y+dy && y+dy < height &&
                map[next_level].at(x+dx, y+dy) == 0 &&
                visited[next_level].at(x+dx, y+dy) == 0) {
                queue.push_back({x+dx, y+dy, t+1});
                visited[next_level].at(x+dx, y+dy) = 1;
            }
        }
    }
    return -1;
}

int main() {
    auto const [width, height, blizzards] = parse_input(std::cin);

    auto const map = build_3d_map(width, height, blizzards);
    // +1 to include moving into the "exit" square outside the map
    long const to_end = search(map, 0, {0,0}, {width-1,height-1})+1;
    std::cout << to_end << "\n";
    long const to_start = search(map, to_end, {width-1, height-1}, {0,0})+1;
    long const to_end_again = search(map, to_start, {0,0}, {width-1,height-1})+1;
    std::cout << to_end_again << "\n";
}
