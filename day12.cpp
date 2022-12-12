#include "grid.hpp"
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <functional>

struct coord {
    long x;
    long y;
};

struct input {
    grid<long> map;
    coord start;
    coord end;
};

input parse_input(std::istream& is) {
    grid_builder<long> map;
    coord start{-1, -1};
    coord end{-1, -1};

    std::string line;
    long y = 0;
    while(std::getline(is, line)) {
        for (long x = 0; static_cast<size_t>(x) < line.size(); ++x) {
            if (line[x] == 'S') {
                start.x = x;
                start.y = y;
                map.push_back(0);
            } else if (line[x] == 'E') {
                end.x = x;
                end.y = y;
                map.push_back('z' - 'a');
            } else {
                map.push_back(line[x] - 'a');
            }
        }
        map.end_line();
        ++y;
    }
    return {std::move(map).finish(), start, end};
}

template<class Prio, class T>
class priority_queue {
public:
    using value_type = std::pair<Prio, T>;

    priority_queue() = default;

    [[nodiscard]] bool empty() const {
        return items.empty();
    }

    void push(Prio prio, T item) {
        items.emplace_back(prio, item);
        std::push_heap(items.begin(), items.end(), compare);
    }

    value_type pop() {
        std::pop_heap(items.begin(), items.end(), compare);
        value_type ret = items.back();
        items.pop_back();
        return ret;
    }

private:
    std::deque<value_type> items;
    static bool compare(value_type const& v1, value_type const& v2) {
        return v1.first > v2.first;
    }
};

std::vector<coord> neighbours(coord pos, long width, long height) {
    std::vector<coord> neighbours;
    neighbours.reserve(4);
    if (pos.x > 0) {
        neighbours.push_back({pos.x-1, pos.y});
    }
    if (pos.y > 0) {
        neighbours.push_back({pos.x, pos.y-1});
    }
    if (pos.x+1 < width) {
        neighbours.push_back({pos.x+1, pos.y});
    }
    if (pos.y+1 < height) {
        neighbours.push_back({pos.x, pos.y+1});
    }
    return neighbours;
}

grid<long> dijkstra(grid<long> const& map, coord start, std::function<bool(coord, coord)> const& filter) {
    grid<long> distance(map.width(), map.height(), std::numeric_limits<long>::max());
    priority_queue<long, coord> pq;
    pq.push(0, start);

    while (!pq.empty()) {
        auto [dist, pos] = pq.pop();
        if (dist < distance.at(pos.x, pos.y)) {
            distance.at(pos.x, pos.y) = dist;

            for (auto n : neighbours(pos, map.width(), map.height())) {
                if (filter(pos, n) && distance.at(n.x, n.y) > dist+1) {
                    pq.push(dist+1, n);
                }
            }
        }
    }
    return distance;
}

int main() {
    auto const [map, start, end] = parse_input(std::cin);

    auto distances = dijkstra(map, start, [&](coord src, coord dst) {
        return map.at(dst.x, dst.y) <= map.at(src.x, src.y)+1;
    });
    std::cout << distances.at(end.x, end.y) << "\n";

    auto reverse_distances = dijkstra(map, end, [&](coord src, coord dst) {
        return map.at(dst.x, dst.y)+1 >= map.at(src.x, src.y);
    });
    std::vector<long> start_distances;
    for (long y = 0; y < map.height(); ++y) {
        for (long x = 0; x < map.width(); ++x) {
            if (map.at(x, y) == 0 && reverse_distances.at(x, y)) {
                start_distances.push_back(reverse_distances.at(x, y));
            }
        }
    }
    auto it = std::min_element(start_distances.begin(), start_distances.end());
    std::cout << (it != start_distances.end() ? *it : -1) << "\n";
}
