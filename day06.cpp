#include <iostream>
#include <string>
#include <unordered_set>

std::string read_input(std::istream& is) {
    std::string line;
    std::getline(is, line);
    return line;
}

size_t count_different(std::string const& s) {
    std::unordered_set<char> counts;
    for (char c : s) {
        counts.insert(c);
    }
    return counts.size();
}

size_t find_marker(std::string const& s) {
    for (size_t i = 0; i + 4 < s.size(); ++i) {
        if (count_different(s.substr(i, 4)) == 4) {
            return i + 4;
        }
    }
    return std::string::npos;
}

int main() {
    std::string const input = read_input(std::cin);

    std::cout << find_marker(input) << "\n";
}