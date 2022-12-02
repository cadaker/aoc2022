#include "algorithm.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include <iterator>

using input_type = std::vector<std::vector<long>>;

static input_type read_input(std::istream& is) {
    input_type input;
    std::string line;
    std::vector<long> block;
    while (std::getline(is, line)) {
        if (!line.empty()) {
            block.push_back(std::stol(line));
        } else if (!block.empty()) {
            input.push_back(std::move(block));
            block = {};
        }
    }
    if (!block.empty()) {
        input.push_back(std::move(block));
    }
    return input;
}

long total_calories(std::vector<long> const& elf) {
    return std::accumulate(elf.begin(), elf.end(), 0L);
}

template<std::forward_iterator Iterator>
Iterator take_n(size_t n, Iterator begin, Iterator end) {
    for (size_t i = 0; i < n && begin != end; ++i, ++begin) {
    }
    return begin;
}

int main() {
    auto input = read_input(std::cin);

    auto calories = map(input, total_calories);

    std::sort(calories.begin(), calories.end());

    std::cout << (calories.empty() ? 0 : calories.back()) << "\n";

    std::cout << std::accumulate(calories.rbegin(), take_n(3, calories.rbegin(), calories.rend()), 0L) << "\n";

    return 0;
}
