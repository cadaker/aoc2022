#include <iostream>
#include <vector>
#include <variant>
#include <memory>
#include <algorithm>

struct msg;
using msg_vector = std::vector<msg>;

struct msg {
    explicit msg(long x): elem(x) {}
    explicit msg(msg_vector v): elem(std::move(v)) {}
    msg(): elem(msg_vector{}) {}

    std::variant<long, msg_vector> elem;
};

msg parse_msg(std::string const& line) {
    if (line.empty() || line[0] != '[') {
        throw std::invalid_argument("unexpected start of msg line: " + line);
    }
    msg ret;

    std::vector<msg_vector*> stack = {&get<msg_vector>(ret.elem)};
    size_t i = 1;
    while (i < line.size() && !stack.empty()) {
        msg_vector* cur_list = stack.back();
        if (line[i] == ']') {
            stack.pop_back();
            ++i;
        } else if(line[i] == ',') {
            ++i;
        } else if (line[i] == '[') {
            cur_list->push_back(msg{});
            msg_vector* new_list = &get<msg_vector>(cur_list->back().elem);
            stack.push_back(new_list);
            ++i;
        } else if (std::isdigit(line[i])) {
            long const x = std::stol(line.substr(i));
            cur_list->push_back(msg{x});
            while (i < line.size() && std::isdigit(line[i])) {
                ++i;
            }
        }
    }
    return ret;
}

std::vector<std::pair<msg, msg>> parse_input(std::istream& is) {
    std::vector<std::pair<msg, msg>> msgs;

    std::string line;
    while(std::getline(is, line)) {
        auto msg1 = parse_msg(line);
        std::getline(is, line);
        auto msg2 = parse_msg(line);
        std::getline(is, line);
        msgs.emplace_back(std::move(msg1), std::move(msg2));
    }
    return msgs;
}

int compare(msg const& m1, msg const& m2) {
    if (get_if<long>(&m1.elem) || get_if<long>(&m2.elem)) {
        throw std::invalid_argument("can only compare root msg vectors");
    }

    using iterator = msg_vector::const_iterator;

    struct state {
        explicit state(msg_vector const& v): first(v.begin()), last(v.end()) {}
        explicit state(iterator f, iterator l): first(f), last(l) {}
        iterator first;
        iterator last;
    };
    std::vector<state> stack1 = {state{std::get<msg_vector>(m1.elem)}};
    std::vector<state> stack2 = {state{std::get<msg_vector>(m2.elem)}};

    while (!stack1.empty() && !stack2.empty()) {
        state& state1 = stack1.back();
        state& state2 = stack2.back();

        if (state1.first != state1.last && state2.first != state2.last) {
            long const* val1 = get_if<long>(&state1.first->elem);
            long const* val2 = get_if<long>(&state2.first->elem);
            msg_vector const* vec1 = get_if<msg_vector>(&state1.first->elem);
            msg_vector const* vec2 = get_if<msg_vector>(&state2.first->elem);

            // Careful, so we don't invalidate pointers.
            auto first1 = state1.first;
            auto first2 = state2.first;
            ++state1.first;
            ++state2.first;

            if (val1 && val2) {
                if (*val1 < *val2) {
                    return -1;
                } else if(*val1 > *val2) {
                    return 1;
                }
            } else if (val1 && vec2) {
                stack1.emplace_back(first1, first1+1);
                stack2.emplace_back(*vec2);
            } else if (vec1 && val2) {
                stack1.emplace_back(*vec1);
                stack2.emplace_back(first2, first2+1);
            } else {
                stack1.emplace_back(*vec1);
                stack2.emplace_back(*vec2);
            }
        } else if (state2.first != state2.last) {
            return -1;
        } else if (state1.first != state1.last) {
            return 1;
        } else {
            stack1.pop_back();
            stack2.pop_back();
        }
    }
    if (stack1.empty() && stack2.empty()) {
        return 0;
    } else if (stack1.empty()) {
        return -1;
    } else {
        return 1;
    }
}

int main() {
    auto const input = parse_input(std::cin);

    size_t index_sum = 0;
    for (size_t i = 0; i < input.size(); ++i) {
        if (compare(input[i].first, input[i].second) <= 0) {
            index_sum += (i+1);
        }
    }
    std::cout << index_sum << "\n";
}
