#include "algorithm.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

std::vector<long> parse_input(std::istream& is) {
    std::vector<long> ret;
    std::string line;
    while (std::getline(is, line)) {
        ret.push_back(std::stol(line));
    }
    return ret;
}

static long modulo(long x, long n) {
    return ((x % n) + n) % n;
}

template<class T>
class cyclic_buffer {
public:
    explicit cyclic_buffer(std::vector<T> v)
            : items(std::move(v))
    {
        for (size_t i = 0; i < items.size(); ++i) {
            item_positions[items[i]] = i;
        }
    }

    void shift_item(T x, long dx) {
        long const n = static_cast<long>(items.size());
        size_t const old_index = item_positions.at(x);
        size_t const new_index = static_cast<size_t>(modulo(static_cast<long>(old_index) + dx, n-1));

        if (old_index <= new_index) {
            for (size_t i = old_index; i < new_index; ++i) {
                items[i] = items[i+1];
                item_positions[items[i]] = i;
            }
        } else {
            for (size_t i = old_index; i > new_index; --i) {
                items[i] = items[i-1];
                item_positions[items[i]] = i;
            }
        }
        items[new_index] = x;
        item_positions[x] = new_index;
    }

    std::vector<T> const& get_items() const {
        return items;
    }

    size_t find(T x) const {
        return item_positions.at(x);
    }

private:
    std::vector<T> items;
    std::unordered_map<T, size_t> item_positions;
};

cyclic_buffer<size_t> index_buffer(size_t n) {
    std::vector<size_t> ret(n);
    for (size_t i = 0; i < n; ++i) {
        ret[i] = i;
    }
    return cyclic_buffer{std::move(ret)};
}

void mix(std::vector<long> const& input, cyclic_buffer<size_t>& buffer) {
    for (size_t i = 0; i < input.size(); ++i) {
        buffer.shift_item(i, input[i]);
    }
}

long coordinates(std::vector<long> const& input, cyclic_buffer<size_t> const& buffer) {
    size_t const input_index0 = std::find(input.begin(), input.end(), 0) - input.begin();
    long const buffer_index0 = static_cast<long>(buffer.find(input_index0));
    long const n = static_cast<long>(input.size());
    return input.at(buffer.get_items().at(modulo(buffer_index0+1000, n))) +
           input.at(buffer.get_items().at(modulo(buffer_index0+2000, n))) +
           input.at(buffer.get_items().at(modulo(buffer_index0+3000, n)));
}

int main() {
    auto const input = parse_input(std::cin);

    auto buffer = index_buffer(input.size());
    mix(input, buffer);

    std::cout << coordinates(input, buffer) << "\n";

    constexpr long KEY = 811589153;

    std::vector<long> const scrambled_input = map(input, [KEY](long x) { return x * KEY; });
    buffer = index_buffer(input.size());
    for (size_t iter = 0; iter < 10; ++iter) {
        mix(scrambled_input, buffer);
    }
    std::cout << coordinates(scrambled_input, buffer) << "\n";
}
