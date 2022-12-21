#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <variant>
#include <unordered_map>

enum class operator_t {
        ADD,
        SUB,
        MUL,
        DIV,
};

struct operation_t {
    operator_t op{};
    std::string lhs;
    std::string rhs;
};

struct value_t {
    long value;
};

using entry_t = std::variant<operation_t, value_t>;

std::unordered_map<std::string, entry_t> parse_input(std::istream& is) {
    std::regex const value_pattern("([a-z]+): (-?[0-9]+)");
    std::regex const add_pattern("([a-z]+): ([a-z]+) \\+ ([a-z]+)");
    std::regex const sub_pattern("([a-z]+): ([a-z]+) - ([a-z]+)");
    std::regex const mul_pattern("([a-z]+): ([a-z]+) \\* ([a-z]+)");
    std::regex const div_pattern("([a-z]+): ([a-z]+) / ([a-z]+)");
    std::unordered_map<std::string, entry_t> input;
    std::string line;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, value_pattern)) {
            input[m[1]] = value_t{std::stol(m[2])};
        } else if (std::regex_match(line, m, add_pattern)) {
            input[m[1]] = operation_t{operator_t::ADD, m[2], m[3]};
        } else if (std::regex_match(line, m, sub_pattern)) {
            input[m[1]] = operation_t{operator_t::SUB, m[2], m[3]};
        } else if (std::regex_match(line, m, mul_pattern)) {
            input[m[1]] = operation_t{operator_t::MUL, m[2], m[3]};
        } else if (std::regex_match(line, m, div_pattern)) {
            input[m[1]] = operation_t{operator_t::DIV, m[2], m[3]};
        } else {
            std::cerr << "failed to parse line: " << line << std::endl;
        }
    }
    return input;
}

long eval_operator(operator_t op, long lhs, long rhs) {
    switch (op) {
        case operator_t::ADD: return lhs + rhs;
        case operator_t::SUB: return lhs - rhs;
        case operator_t::MUL: return lhs * rhs;
        case operator_t::DIV: return lhs / rhs;
    }
    throw std::invalid_argument("no such operator");
}

long evaluate(std::unordered_map<std::string, entry_t> const& entries, std::string const& root) {
    std::vector<std::string> stack = {root};
    std::unordered_map<std::string, long> evaluated;
    while (!stack.empty()) {
        auto const& name = stack.back();
        if (auto* value = std::get_if<value_t>(&entries.at(name))) {
            evaluated[name] = value->value;
            stack.pop_back();
        } else if (auto* op = get_if<operation_t>(&entries.at(name))) {
            auto lhs = evaluated.find(op->lhs);
            if (lhs == evaluated.end()) {
                stack.emplace_back(op->lhs);
                continue;
            }
            auto rhs = evaluated.find(op->rhs);
            if (rhs == evaluated.end()) {
                stack.emplace_back(op->rhs);
                continue;
            }
            evaluated[name] = eval_operator(op->op, lhs->second, rhs->second);
            stack.pop_back();
        }
    }
    return evaluated.at(root);
}

int main() {
    auto input = parse_input(std::cin);

    std::cout << evaluate(input, "root") << "\n";
}
