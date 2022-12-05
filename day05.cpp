#include <iostream>
#include <string>
#include <vector>
#include <regex>

struct operation_t {
    size_t count;
    size_t from;
    size_t to;
};

using stack_t = std::vector<char>;
using stacks_t = std::vector<stack_t>;

std::pair<stacks_t, std::vector<operation_t>> parse_input(std::istream& is) {
    std::string line;

    // Initial stacks
    // We build the stacks upside down, and then flip them over at the end
    stacks_t stacks;
    while (std::getline(is, line) && !line.empty()) {
        for (size_t i = 1; i < line.size(); i += 4) {
            if (line[i] == ' ' || ('0' <= line[i] && line[i] <= '9')) {
                // Skip
            } else if ('A' <= line[i] && line[i] <= 'Z') {
                size_t const column = i / 4;
                stacks.resize(std::max(column+1, stacks.size()));
                stacks.at(column).push_back(line[i]);
            } else {
                std::cerr << "Invalid stacks input: " << line << std::endl;
            }
        }
    }
    // Flip the stacks right again.
    for (stack_t& stack : stacks) {
        std::reverse(stack.begin(), stack.end());
    }

    // Parse the operations
    std::regex const pattern(R"(move (\d+) from (\d) to (\d))");
    std::vector<operation_t> operations;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, pattern)) {
            size_t const count = std::stoul(m[1]);
            size_t const from = std::stoul(m[2]) - 1;
            size_t const to = std::stoul(m[3]) - 1;
            operations.push_back({count, from, to});
        } else {
            std::cerr << "Invalid operations input: " << line << std::endl;
        }
    }
    return {std::move(stacks), std::move(operations)};
}

void move_crates(stacks_t& stacks, operation_t const& op, bool flip_order) {
    if (op.from >= stacks.size() || op.to >= stacks.size()) {
        std::cerr << "invalid stack size: " << op.count << " " << op.from << " " << op.to << std::endl;
        return;
    } else if (op.count > stacks[op.from].size()) {
        std::cerr << "invalid crate count: " << op.count << " " << op.from << " " << op.to << std::endl;
        return;
    }

    auto& src_stack = stacks[op.from];
    auto& dst_stack = stacks[op.to];

    auto start = src_stack.begin() + static_cast<ssize_t>(src_stack.size() - op.count);
    stack_t const tmp(start, src_stack.end());
    src_stack.erase(start, src_stack.end());
    if (flip_order) {
        dst_stack.insert(dst_stack.end(), tmp.rbegin(), tmp.rend());
    } else {
        dst_stack.insert(dst_stack.end(), tmp.begin(), tmp.end());
    }
}

void run_crane(stacks_t stacks, std::vector<operation_t> const& ops, bool flip_order, std::ostream& out) {
    for (auto const& op : ops) {
        move_crates(stacks, op, flip_order);
    }
    for (stack_t const& s : stacks) {
        out << (s.empty() ? '!' : s.back());
    }
    out << std::endl;
}

int main() {
    auto const [stacks, operations] = parse_input(std::cin);

    run_crane(stacks, operations, true, std::cout);
    run_crane(stacks, operations, false, std::cout);
}
