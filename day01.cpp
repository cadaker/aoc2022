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
    return std::accumulate(elf.begin(), elf.end(), 0L, std::plus<>{});
}

template<std::forward_iterator Iterator>
Iterator take_n(size_t n, Iterator begin, Iterator end) {
    for (size_t i = 0; i < n && begin != end; ++i, ++begin) {
    }
    return begin;
}

int main() {
    auto input = read_input(std::cin);

    std::sort(input.begin(), input.end(), [](auto const& e0, auto const& e1) {
        return total_calories(e0) > total_calories(e1);
    });

    std::cout << (input.empty() ? 0 : total_calories(input.front())) << "\n";

    std::cout << std::accumulate(input.begin(), take_n(3, input.begin(), input.end()), 0L, [](long acc, std::vector<long> const& xs) {
        return acc + total_calories(xs);
    }) << "\n";

    return 0;
}
