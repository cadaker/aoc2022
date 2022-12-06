#include <iostream>
#include <string>
#include <unordered_set>

std::string read_input(std::istream& is) {
    std::string line;
    std::getline(is, line);
    return line;
}

size_t count_different(std::string const& s) {
    std::unordered_set<char> counts(s.begin(), s.end());
    return counts.size();
}

size_t find_marker(std::string const& s, size_t len) {
    for (size_t i = 0; i + len < s.size(); ++i) {
        if (count_different(s.substr(i, len)) == len) {
            return i + len;
        }
    }
    return std::string::npos;
}

int main() {
    std::string const input = read_input(std::cin);

    std::cout << find_marker(input, 4) << "\n";
    std::cout << find_marker(input, 14) << "\n";
}
