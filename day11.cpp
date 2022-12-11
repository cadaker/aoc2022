#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>
#include <numeric>

struct monkey_t {
    long id = -1;
    std::deque<long> items;
    std::function<long(long)> operation;
    long divisible_test = 0;
    long true_target = 0;
    long false_target = 0;
};

bool fail(std::string const& line) {
    std::cerr << "Failed to parse line: " << line << std::endl;
    return false;
}

bool parse_monkey(std::istream& is, monkey_t& monkey) {
    std::regex const monkey_pattern(R"(Monkey ([0-9]+):)");
    std::regex const items_pattern(R"(  Starting items: [0-9, ]+)");
    std::regex const item_search_pattern(R"([0-9]+)");
    std::regex const operation_pattern(R"(  Operation: new = old ([+*]) (old|[0-9]+))");
    std::regex const test_pattern(R"(  Test: divisible by ([0-9]+))");
    std::regex const true_pattern(R"(    If true: throw to monkey ([0-9]+))");
    std::regex const false_pattern(R"(    If false: throw to monkey ([0-9]+))");

    std::vector<std::string> lines(6);
    for (std::string& line : lines) {
        if (!std::getline(is, line)) {
            return false;
        }
    }

    std::smatch m;

    if (std::regex_match(lines[0], m, monkey_pattern)) {
        monkey.id = std::stol(m[1]);
    } else {
        return fail(lines[0]);
    }

    if (std::regex_match(lines[1], m, items_pattern)) {
        auto begin = std::sregex_iterator{lines[1].begin(), lines[1].end(), item_search_pattern};
        auto end = std::sregex_iterator{};
        for (auto it = begin; it != end; ++it) {
            monkey.items.push_back(std::stol(it->str()));
        }
    } else {
        return fail(lines[1]);
    }

    if (std::regex_match(lines[2], m, operation_pattern)) {
        if (m[1] == "+") {
            if (m[2] == "old") {
                monkey.operation = [](long x) { return x + x; };
            } else {
                monkey.operation = [y = std::stol(m[2])](long x) { return x + y; };
            }
        } else {
            if (m[2] == "old") {
                monkey.operation = [](long x) { return x * x; };
            } else {
                monkey.operation = [y = std::stol(m[2])](long x) { return x * y; };
            }
        }
    } else {
        return fail(lines[2]);
    }

    if (std::regex_match(lines[3], m, test_pattern)) {
        monkey.divisible_test = std::stol(m[1]);
    } else {
        return fail(lines[3]);
    }

    if (std::regex_match(lines[4], m, true_pattern)) {
        monkey.true_target = std::stol(m[1]);
    } else {
        return fail(lines[4]);
    }

    if (std::regex_match(lines[5], m, false_pattern)) {
        monkey.false_target = std::stol(m[1]);
    } else {
        return fail(lines[5]);
    }

    return true;
}

std::vector<monkey_t> parse_input(std::istream& is) {

    std::vector<monkey_t> input;

    while (true) {
        monkey_t monkey{};
        if (parse_monkey(is, monkey)) {
            input.push_back(std::move(monkey));
            std::string line;
            std::getline(is, line);
            if (!line.empty()) {
                std::cerr << "Expected empty line, got: "<< line << std::endl;
                return {};
            }
        } else {
            return input;
        }
    }
}

using worry_reduction_function = std::function<long(long)>;

long process_monkey(std::vector<monkey_t>& monkeys, worry_reduction_function const& worry_reduction, monkey_t& current_monkey) {
    long items_processed = 0;
    while (!current_monkey.items.empty()) {
        long item = current_monkey.items.front();
        current_monkey.items.pop_front();
        item = worry_reduction(current_monkey.operation(item));
        if (item % current_monkey.divisible_test == 0) {
            monkeys.at(current_monkey.true_target).items.push_back(item);
        } else {
            monkeys.at(current_monkey.false_target).items.push_back(item);
        }
        ++items_processed;
    }
    return items_processed;
}

void monkeys_round(std::vector<monkey_t>& monkeys, worry_reduction_function const& worry_reduction, std::vector<long>& monkey_activities) {
    for (size_t i = 0; i < monkeys.size(); ++i) {
        monkey_activities.at(i) += process_monkey(monkeys, worry_reduction, monkeys.at(i));
    }
}

std::vector<long> process_monkeys(std::vector<monkey_t> monkeys, worry_reduction_function const& worry_reduction, size_t rounds) {
    std::vector<long> activities(monkeys.size());
    for (size_t round = 0; round < rounds; ++round) {
        monkeys_round(monkeys, worry_reduction, activities);
    }
    return activities;
}

long monkey_business(std::vector<long> monkey_activities) {
    if (monkey_activities.empty()) {
        return 0;
    }
    auto first = std::max_element(monkey_activities.begin(), monkey_activities.end());
    long const best_monkey = *first;
    *first = -1;
    auto second = std::max_element(monkey_activities.begin(), monkey_activities.end());
    return best_monkey * *second;
}

int main() {
    auto monkeys = parse_input(std::cin);

    auto activities20 = process_monkeys(monkeys, [](long worry) { return worry / 3; }, 20);
    std::cout << monkey_business(activities20) << "\n";

    long const worry_modulus = std::accumulate(monkeys.begin(), monkeys.end(), 1, [](long acc, monkey_t const& m) {
        return acc * m.divisible_test;
    });

    auto activities10000 = process_monkeys(monkeys, [worry_modulus](long worry) { return worry % worry_modulus; }, 10000);
    std::cout << monkey_business(activities10000) << "\n";
}
