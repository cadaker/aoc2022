#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>
#include <array>
#include <algorithm>

struct coord_t {
    long x = 0;
    long y = 0;
    long z = 0;
};

class point_set_t {
public:
    point_set_t() = default;

    point_set_t(std::vector<coord_t> const& points) {
        for (coord_t xyz : points) {
            add(xyz);
        }
    }

    bool contains(coord_t xyz) const {
        auto itx = xyzs.find(xyz.x);
        if (itx != xyzs.end()) {
            auto ity = itx->second.find(xyz.y);
            if (ity != itx->second.end()) {
                auto itz = ity->second.find(xyz.z);
                if (itz != ity->second.end()) {
                    return itz->second;
                }
            }
        }
        return false;
    }

    void add(coord_t xyz) {
        xyzs[xyz.x][xyz.y][xyz.z] = true;
    }

private:
    std::unordered_map<long, std::unordered_map<long, std::unordered_map<long, bool>>> xyzs;
};

std::vector<coord_t> parse_input(std::istream& is) {
    std::vector<coord_t> points;
    std::regex const pattern(R"(([-0-9]+),([-0-9]+),([-0-9]+))");
    std::string line;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, pattern)) {
            points.push_back({std::stol(m[1]), std::stol(m[2]), std::stol(m[3])});
        } else {
            std::cerr << "malformed line: " << line << "\n";
        }
    }
    return points;
}

std::array<coord_t, 6> neighbours(coord_t xyz) {
    return {{
            {xyz.x - 1, xyz.y, xyz.z},
            {xyz.x + 1, xyz.y, xyz.z},
            {xyz.x, xyz.y - 1, xyz.z},
            {xyz.x, xyz.y + 1, xyz.z},
            {xyz.x, xyz.y, xyz.z - 1},
            {xyz.x, xyz.y, xyz.z + 1},
    }};
}

long count_exposed_area(std::vector<coord_t> const& xyzs, point_set_t const& point_set, bool count_touching) {
    long count = 0;
    for (auto const& xyz : xyzs) {
        for (auto n : neighbours(xyz)) {
            if ((count_touching && point_set.contains(n)) || (!count_touching && !point_set.contains(n))) {
                count++;
            }
        }
    }
    return count;
}

std::pair<coord_t, coord_t> bounds(std::vector<coord_t> const& points) {
    auto minx = std::min_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.x < c2.x; });
    auto maxx = std::max_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.x < c2.x; });
    auto miny = std::min_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.y < c2.y; });
    auto maxy = std::max_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.y < c2.y; });
    auto minz = std::min_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.z < c2.z; });
    auto maxz = std::max_element(points.begin(), points.end(), [](auto & c1, auto& c2) { return c1.z < c2.z; });
    if (points.empty()) {
        return {{0,0,0},{0,0,0}};
    } else {
        return {{minx->x, miny->y, minz->z}, {maxx->x, maxy->y, maxz->z}};
    }
}

point_set_t find_outside_points(std::vector<coord_t> const& points) {
    point_set_t const point_set(points);
    point_set_t outside_points;

    auto [mn, mx] = bounds(points);
    coord_t const start_point {mn.x-1, mn.y-1, mn.z-1};
    outside_points.add(start_point);

    std::vector<coord_t> points_to_visit{start_point};

    while (!points_to_visit.empty()) {
        coord_t const xyz = points_to_visit.back();
        points_to_visit.pop_back();
        for (coord_t const n : neighbours(xyz)) {
            if (!outside_points.contains(n) &&
                !point_set.contains(n) &&
                n.x >= mn.x-1 && n.x <= mx.x+1 &&
                n.y >= mn.y-1 && n.y <= mx.y+1 &&
                n.z >= mn.z-1 && n.z <= mx.z+1) {
                points_to_visit.push_back(n);
                outside_points.add(n);
            }
        }
    }
    return outside_points;
}

int main() {
    auto input = parse_input(std::cin);

    point_set_t point_set(input);

    std::cout << count_exposed_area(input, point_set, false) << "\n";

    auto const outside_points = find_outside_points(input);
    std::cout << count_exposed_area(input, outside_points, true) << "\n";
}
