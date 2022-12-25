#include "algorithm.hpp"
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> parse_input(std::istream& is) {
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(is, line)) {
        lines.push_back(std::move(line));
    }
    return lines;
}

long snafu_to_long(std::string const& snafu) {
    long value = 0;
    for (char c : snafu) {
        value *= 5;
        switch (c) {
            case '1': value += 1; break;
            case '2': value += 2; break;
            case '-': value -= 1; break;
            case '=': value -= 2; break;
            default: break;
        }
    }
    return value;
}

std::string long_to_base5(long n) {
    if (n == 0) {
        return "0";
    }
    std::vector<char> reverse_digits;
    while (n > 0) {
        reverse_digits.push_back(static_cast<char>('0' + (n%5)));
        n /= 5;
    }
    return std::string{reverse_digits.rbegin(), reverse_digits.rend()};
}

std::string long_to_snafu(long n) {
    // The idea here is that adding the number 222...2 (base 5) to a snafu number gives a
    // nice number base 5. And then we can easily subtract away 222...2 (base 5) and get
    // the snafu representation.
    long offset = 0;
    long tmp = n;
    while (tmp > 0) {
        offset *= 5;
        offset += 2;
        tmp /= 5;
    }
    std::string const offset_base_5 = long_to_base5(n + offset);
    std::string snafu(offset_base_5.size(), '0');
    for (size_t i = 0; i < offset_base_5.size(); ++i) {
        switch (offset_base_5[i]) {
            case '0': snafu[i] = '='; break;
            case '1': snafu[i] = '-'; break;
            case '2': snafu[i] = '0'; break;
            case '3': snafu[i] = '1'; break;
            case '4': snafu[i] = '2'; break;
        }
    }
    return snafu;
}

int main() {
    auto const input = parse_input(std::cin);
    auto const nums = map(input, snafu_to_long);
    auto const sum = std::accumulate(nums.begin(), nums.end(), 0L);
    std::cout << long_to_snafu(sum) << "\n";
}
