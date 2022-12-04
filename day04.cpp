#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>

struct range {
    int start = 0;
    int end = 0;
};

std::vector<std::pair<range, range>> parse_input(std::istream& is) {
    std::regex const pattern(R"(^(\d+)-(\d+),(\d+)-(\d+)$)");
    std::string line;
    std::vector<std::pair<range, range>> ret;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, pattern)) {
            ret.emplace_back(range{std::stoi(m[1]), std::stoi(m[2])}, range{std::stoi(m[3]), std::stoi(m[4])});
        } else {
            std::cerr << "Invalid input line: " << line << "\n";
        }
    }
    return ret;
}

bool contains(range const& container, range const& containee) {
    return container.start <= containee.start && containee.end <= container.end;
}

bool either_contains(std::pair<range, range> const& p) {
    return contains(p.first, p.second) || contains(p.second, p.first);
}

int main() {
    auto input = parse_input(std::cin);

    std::cout << std::count_if(input.begin(), input.end(), either_contains) << "\n";
}
