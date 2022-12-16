#include <iostream>
#include <vector>
#include <unordered_map>
#include <regex>
#include <string>
#include <algorithm>
#include <iterator>
#include <limits>

struct valve_t {
    long flow = 0;
    size_t id = 0;
    std::string name;
    std::vector<size_t> exits;
};

using valves_t = std::vector<valve_t>;

class index_mapper {
public:
    index_mapper() = default;
    size_t map(std::string const& s) {
        auto it = mapping.find(s);
        if (it != mapping.end()) {
            return it->second;
        } else {
            return mapping[s] = next_index++;
        }
    }

private:
    size_t next_index = 0;
    std::unordered_map<std::string, size_t> mapping;
};

valves_t parse_input(std::istream& is, index_mapper& mapper) {
    std::regex const line_pattern(R"(Valve ([A-Z]{2}) has flow rate=([0-9]+); tunnels? leads? to valves? (.*))");
    std::regex const exit_pattern(R"(([A-Z]{2}))");

    valves_t valves;
    std::string line;
    while (std::getline(is, line)) {
        std::smatch m;
        valve_t valve;
        if (std::regex_match(line, m, line_pattern)) {
            valve.name = m[1];
            valve.id = mapper.map(m[1]);
            valve.flow = std::stol(m[2]);
            std::transform(
                    std::sregex_iterator{m[3].first, m[3].second, exit_pattern},
                    std::sregex_iterator{},
                    std::back_inserter(valve.exits),
                    [&mapper](std::smatch const& m) {
                        return mapper.map(m[1]);
                    });
            valves.push_back(std::move(valve));
        } else {
            std::cout << "malformed line: " << line << "\n";
            return {};
        }
    }
    std::sort(valves.begin(), valves.end(), [](auto const&v1, auto const& v2) {
        return v1.id < v2.id;
    });
    return valves;
}

class valve_set {
public:
    valve_set() = default;

    void add(size_t i) {
        if (i >= 64) {
            throw std::out_of_range("invalid set index");
        }
        bitmap |= (1 << i);
    }

    [[nodiscard]] bool contains(size_t i) const {
        if (i >= 64) {
            throw std::out_of_range("invalid set index");
        }
        return bitmap & (1 << i);
    }

    [[nodiscard]] size_t size() const {
        size_t sz = 0;
        for (size_t i = 0; i < 64; ++i) {
            sz += (bitmap & (1 << i)) != 0;
        }
        return sz;
    }

private:
    uint64_t bitmap;
};

using distances_t = std::unordered_map<size_t, std::unordered_map<size_t, long>>;

distances_t compute_distances(valves_t const& valves) {
    constexpr long inf = std::numeric_limits<long>::max();
    // Floyd-Warshal
    distances_t distances;
    for (size_t i = 0; i < valves.size(); ++i) {
        for (size_t j = 0; j < valves.size(); ++j) {
            distances[i][j] = inf;
        }
    }
    for (valve_t const& valve : valves) {
        for (size_t exit : valve.exits) {
            distances[valve.id][exit] = 1;
        }
        distances[valve.id][valve.id] = 0;
    }

    for (size_t k = 0; k < valves.size(); ++k) {
        for (size_t i = 0; i < valves.size(); ++i) {
            for (size_t j = 0; j < valves.size(); ++j) {
                if (distances[i][k] != inf && distances[k][j] != inf) {
                    distances[i][j] = std::min(distances[i][j], distances[i][k] + distances[k][j]);
                }
            }
        }
    }
    return distances;
}

struct state_t {
    long current_time = 0;
    size_t current_pos = 0;
    valve_set open_valves{};
    long total_flow = 0;
};

long search(valves_t const& valves, distances_t const& distances, size_t start_id) {
    long best_flow = 0;
    std::deque<state_t> queue = {state_t{30, start_id, {}, 0}};
    while (!queue.empty()) {
        state_t state = std::move(queue.front());
        queue.pop_front();
        if (state.total_flow > best_flow) {
            best_flow = state.total_flow;
        }
        long const current_valve_flow = valves.at(state.current_pos).flow;
        if (state.current_time > 0) {
            if (current_valve_flow > 0 && !state.open_valves.contains(state.current_pos)) {
                state_t new_state = state;
                new_state.current_time--;
                new_state.open_valves.add(new_state.current_pos);
                new_state.total_flow += current_valve_flow * new_state.current_time;
                queue.push_back(new_state);
            } else {
                for (valve_t const& valve : valves) {
                    long const distance = distances.at(state.current_pos).at(valve.id);
                    if (valve.flow > 0 && !state.open_valves.contains(valve.id) && distance < state.current_time) {
                        state_t new_state = state;
                        new_state.current_time -= distance;
                        new_state.current_pos = valve.id;
                        queue.push_back(new_state);
                    }
                }
            }
        }
    }
    return best_flow;
}

int main() {
    index_mapper mapper;
    auto valves = parse_input(std::cin, mapper);
    distances_t const distances = compute_distances(valves);

    std::cout << search(valves, distances, mapper.map("AA")) << "\n";
}
