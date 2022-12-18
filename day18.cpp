#include <iostream>
#include <string>
#include <regex>
#include <unordered_map>
#include <array>

struct coord_t {
    long x = 0;
    long y = 0;
    long z = 0;
};

class point_set_t {
public:
    point_set_t() = default;

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

long count_exposed_area(std::vector<coord_t> const& xyzs) {
    point_set_t point_set;
    for (auto const& xyz : xyzs) {
        point_set.add(xyz);
    }
    long count = 0;
    for (auto const& xyz : xyzs) {
        count += 6;
        for (auto n : neighbours(xyz)) {
            if (point_set.contains(n)) {
                count--;
            }
        }
    }
    return count;
}

int main() {
    auto input = parse_input(std::cin);

    std::cout << count_exposed_area(input) << "\n";
}
