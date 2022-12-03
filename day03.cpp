#include "algorithm.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

std::vector<std::string> parse_input(std::istream& is) {
    std::string line;
    std::vector<std::string> ret;
    while (std::getline(is, line)) {
        ret.push_back(line);
    }
    return ret;
}

int item_priority(char item) {
    if ('a' <= item && item <= 'z') {
        return item - 'a' + 1;
    } else {
        return item - 'A' + 27;
    }
}

int backpack_priority(std::string const& backpack) {
    std::vector<char> comp1(backpack.begin(), backpack.begin() + backpack.size()/2);
    std::vector<char> comp2(backpack.begin() + backpack.size()/2, backpack.end());

    std::sort(comp1.begin(), comp1.end());
    std::sort(comp2.begin(), comp2.end());

    std::vector<char> intersection;
    std::set_intersection(comp1.begin(), comp1.end(), comp2.begin(), comp2.end(), std::back_inserter(intersection));

    if (!intersection.empty()) {
        return item_priority(intersection.front());
    } else {
        return 0;
    }
}

int group_priority(std::string const& backpack1, std::string const& backpack2, std::string const& backpack3) {
    std::vector<char> items1(backpack1.begin(), backpack1.end());
    std::vector<char> items2(backpack2.begin(), backpack2.end());
    std::vector<char> items3(backpack3.begin(), backpack3.end());
    std::sort(items1.begin(), items1.end());
    std::sort(items2.begin(), items2.end());
    std::sort(items3.begin(), items3.end());

    std::vector<char> intersection12;
    std::vector<char> intersection123;
    std::set_intersection(items1.begin(), items1.end(), items2.begin(), items2.end(), std::back_inserter(intersection12));
    std::set_intersection(intersection12.begin(), intersection12.end(), items3.begin(), items3.end(), std::back_inserter(intersection123));

    if (!intersection123.empty()) {
        return item_priority(intersection123.front());
    } else {
        return 0;
    }
}

int main() {
    auto input = parse_input(std::cin);

    std::cout << accumulate_map(input.begin(), input.end(), 0L, backpack_priority) << "\n";

    long group_total = 0;
    for (size_t i = 0; i+2 < input.size(); i += 3) {
        group_total += group_priority(input[i], input[i+1], input[i+2]);
    }
    std::cout << group_total << "\n";
}
