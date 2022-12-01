#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>

using input_type = std::vector<std::vector<long>>;

static input_type read_input(std::istream& is) {
    input_type input;
    std::string line;
    bool has_data = true;
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

int main() {
    auto input = read_input(std::cin);

    auto max_elf = std::max_element(input.begin(), input.end(), [](auto const& e0, auto const& e1) {
        return total_calories(e0) < total_calories(e1);
    });

    if (max_elf != input.end()) {
        std::cout << total_calories(*max_elf) << "\n";
    }

    std::sort(input.begin(), input.end(), [](auto const& e0, auto const& e1) {
        return total_calories(e0) > total_calories(e1);
    });

    long total = 0;
    for (size_t i = 0; i < input.size() && i < 3; ++i) {
        total += total_calories(input[i]);
    }
    std::cout << total << "\n";

    return 0;
}
