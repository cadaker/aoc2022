#include "algorithm.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>
#include <unordered_set>

struct coord {
    long x = 0;
    long y = 0;
};

struct reading {
    coord sensor{};
    coord beacon{};

    [[nodiscard]] long mh_radius() const {
        long const dx = beacon.x - sensor.x;
        long const dy = beacon.y - sensor.y;
        return labs(dx) + labs(dy);
    }
};

std::vector<reading> parse_input(std::istream& is) {
    std::regex const sensor_pattern(R"(Sensor at x=([-0-9]+), y=([-0-9]+))");
    std::regex const beacon_pattern(R"(beacon is at x=([-0-9]+), y=([-0-9]+))");
    std::vector<reading> readings;
    std::string line;
    while (std::getline(is, line)) {
        reading r;
        std::smatch m;
        if (std::regex_search(line, m, sensor_pattern)) {
            r.sensor.x = std::stol(m[1]);
            r.sensor.y = std::stol(m[2]);
        } else {
            std::cerr << "could not parse sensor: " << line << std::endl;
            continue;
        }
        if (std::regex_search(line, m, beacon_pattern)) {
            r.beacon.x = std::stol(m[1]);
            r.beacon.y = std::stol(m[2]);
        } else {
            std::cerr << "could not parse beacon: " << line << std::endl;
            continue;
        }
        readings.push_back(r);
    }
    return readings;
}

struct interval {
    // [begin, end)
    long begin = 0;
    long end = 0;

    [[nodiscard]] bool empty() const {
        return begin >= end;
    }
};

interval cover_at_y(reading const& r, long y) {
    long const dy = labs(r.sensor.y - y);
    long const inner_radius = r.mh_radius();
    if (dy > r.mh_radius()) {
        return interval{};
    }
    long const dx = inner_radius - dy;
    return interval{r.sensor.x - dx, r.sensor.x + dx + 1};
}

const long Y = 2'000'000;

size_t union_size(std::vector<interval> const& intervals) {
    std::unordered_set<long> points;
    for (interval const& intl : intervals) {
        for (long x = intl.begin; x < intl.end; ++x) {
            points.insert(x);
        }
    }
    return points.size();
}

size_t unique_beacons_at(std::vector<reading> const& readings, long y) {
    std::unordered_set<long> xs;
    for (auto const& r : readings) {
        if (r.beacon.y == y) {
            xs.insert(r.beacon.x);
        }
    }
    return xs.size();
}

int main() {
    auto input = parse_input(std::cin);

    std::vector<interval> const intervals = map(input, [](reading const& r) { return cover_at_y(r, Y);});

    std::cout << (union_size(intervals) - unique_beacons_at(input, Y)) << "\n";
}
