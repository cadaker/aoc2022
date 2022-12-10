#include <iostream>
#include <vector>

using op_t = long;

std::vector<op_t> parse_input(std::istream& is) {
    std::vector<op_t> ops;
    std::string line;
    while (getline(is, line)) {
        if (line == "noop") {
            ops.push_back(0);
        } else if (line.starts_with("addx ")) {
            ops.push_back(std::stol(line.substr(5)));
        }
    }
    return ops;
}

std::vector<long> process_signal(std::vector<op_t> const& ops) {
    std::vector<long> signal;
    long x = 1;
    signal.push_back(x);
    for (op_t op : ops) {
        if (op == 0) {
            signal.push_back(x);
        } else {
            signal.push_back(x);
            x += op;
            signal.push_back(x);
        }
    }
    return signal;
}

const size_t samples[] = {20, 60, 100, 140, 180, 220};

int main() {
    auto const ops = parse_input(std::cin);
    auto const signal = process_signal(ops);

    long sum = 0;
    for (size_t i : samples) {
        sum += signal.at(i-1) * i;
    }
    std::cout << sum << "\n";
}
