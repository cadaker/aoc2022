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

int main() {
    auto const input = parse_input(std::cin);
    std::vector<size_t> input_indices(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        input_indices[i] = i;
    }

    cyclic_buffer<size_t> buffer(input_indices);
    for (size_t i = 0; i < input.size(); ++i) {
        buffer.shift_item(i, input[i]);
    }
    size_t const input_index0 = std::find(input.begin(), input.end(), 0) - input.begin();
    long const index0 = static_cast<long>(buffer.find(input_index0));
    long const n = static_cast<long>(input.size());
    std::cout << (
            input.at(buffer.get_items().at(modulo(index0+1000, n))) +
            input.at(buffer.get_items().at(modulo(index0+2000, n))) +
            input.at(buffer.get_items().at(modulo(index0+3000, n)))) << "\n";
}
