#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <variant>
#include <memory>
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

struct node_t;

struct variable_t {};

struct binop_t {
    operator_t op;
    std::unique_ptr<node_t> lhs;
    std::unique_ptr<node_t> rhs;
};

struct node_t {
    std::variant<value_t, variable_t, binop_t> elem;
};

node_t make_binop(operator_t op, node_t lhs, node_t rhs) {
    auto* lhs_value = get_if<value_t>(&lhs.elem);
    auto* rhs_value = get_if<value_t>(&rhs.elem);
    if (lhs_value && rhs_value) {
        return node_t{value_t{eval_operator(op, lhs_value->value, rhs_value->value)}};
    } else {
        return node_t{
                binop_t{op,
                        std::make_unique<node_t>(std::move(lhs)),
                        std::make_unique<node_t>(std::move(rhs))}
        };
    }
}

node_t build_subtree(
        std::unordered_map<std::string, entry_t> const& entries,
        std::string const& subroot,
        std::string const& var_name)
{
    std::vector<std::string> stack = {subroot};
    std::unordered_map<std::string, node_t> nodes;

    while (!stack.empty()) {
        std::string const& name = stack.back();
        auto const& entry = entries.at(name);
        if (name == var_name) {
            nodes[name] = node_t{variable_t{}};
            stack.pop_back();
        } else if (auto* value = get_if<value_t>(&entry)) {
            nodes[name] = node_t{value_t{*value}};
            stack.pop_back();
        } else if (auto* op = get_if<operation_t>(&entry)) {
            auto lhs = nodes.find(op->lhs);
            if (lhs == nodes.end()) {
                stack.push_back(op->lhs);
                continue;
            }
            auto rhs = nodes.find(op->rhs);
            if (rhs == nodes.end()) {
                stack.push_back(op->rhs);
                continue;
            }
            nodes[name] = make_binop(op->op, std::move(nodes.at(op->lhs)), std::move(nodes.at(op->rhs)));
            stack.pop_back();
        }
    }
    return std::move(nodes.at(subroot));
}

std::pair<node_t, node_t> build_tree(
        std::unordered_map<std::string, entry_t> const& entries,
        std::string const& root,
        std::string const& var) {
    std::vector<std::string> stack;
    auto const& root_entry = entries.at(root);
    auto const& op = get<operation_t>(root_entry);
    return {build_subtree(entries, op.lhs, var), build_subtree(entries, op.rhs, var)};
}

long solve(node_t const& root, long expected) {
    node_t const* tree = &root;
    // Processing tree == expected
    while (true) {
        if (get_if<variable_t>(&tree->elem)) {
            return expected;
        } else if (auto op = get_if<binop_t>(&tree->elem)) {
            auto* lhs_value = get_if<value_t>(&op->lhs->elem);
            auto* rhs_value = get_if<value_t>(&op->rhs->elem);
            if (!rhs_value && !lhs_value) {
                std::cerr << "error - expected one side to be simplified" << std::endl;
                return 0;
            }

            if (rhs_value && op->op == operator_t::ADD) {
                // x + a == c  =>  x = c - a
                expected = expected - rhs_value->value;
                tree = op->lhs.get();
            } else if (lhs_value && op->op == operator_t::ADD) {
                // a + x == c  =>  x = c - a
                expected = expected - lhs_value->value;
                tree = op->rhs.get();
            } else if (rhs_value && op->op == operator_t::SUB) {
                // x - a == c  =>  x = c + a
                expected = expected + rhs_value->value;
                tree = op->lhs.get();
            } else if (lhs_value && op->op == operator_t::SUB) {
                // a - x == c  =>  x = a - c
                expected = lhs_value->value - expected;
                tree = op->rhs.get();
            } else if (rhs_value && op->op == operator_t::MUL) {
                // x * a == c  =>  x = c / a
                expected = expected / rhs_value->value;
                tree = op->lhs.get();
            } else if (lhs_value && op->op == operator_t::MUL) {
                // a * x == c  =>  x = c / a
                expected = expected / lhs_value->value;
                tree = op->rhs.get();
            } else if (rhs_value && op->op == operator_t::DIV) {
                // x / a == c  =>  x = c * a
                expected = expected * rhs_value->value;
                tree = op->lhs.get();
            } else if (lhs_value && op->op == operator_t::DIV) {
                // a / x == c  =>  x = a / c
                expected = lhs_value->value / expected;
                tree = op->rhs.get();
            }

        } else {
            std::cerr << "error - found constant == constant" << std::endl;
            return 0;
        }
    }
}

int main() {
    auto input = parse_input(std::cin);

    std::cout << evaluate(input, "root") << "\n";

    auto [lhs, rhs] = build_tree(input, "root", "humn");
    auto const* lhs_value = get_if<value_t>(&lhs.elem);
    auto const* rhs_value = get_if<value_t>(&rhs.elem);
    if (lhs_value) {
        std::cout << solve(rhs, lhs_value->value) << "\n";
    } else if (rhs_value) {
        std::cout << solve(lhs, rhs_value->value) << "\n";
    } else {
        std::cerr << "expected one side to be a basic value" << std::endl;
    }
}
