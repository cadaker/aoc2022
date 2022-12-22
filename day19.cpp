#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <deque>
#include <unordered_map>

struct blueprint {
    long ore_robot_ore_cost = 0;
    long clay_robot_ore_cost = 0;
    long obsidian_robot_ore_cost = 0;
    long obsidian_robot_clay_cost = 0;
    long geode_robot_ore_cost = 0;
    long geode_robot_obsidian_cost = 0;

    [[nodiscard]] long max_ore_cost() const {
        return std::max(
                std::max(ore_robot_ore_cost, clay_robot_ore_cost),
                std::max(obsidian_robot_ore_cost, geode_robot_ore_cost));
    }
};

std::vector<blueprint> parse_input(std::istream& is) {
    std::regex const pattern(
            "Blueprint [0-9]+: Each ore robot costs ([0-9]+) ore. "
            "Each clay robot costs ([0-9]+) ore. "
            "Each obsidian robot costs ([0-9]+) ore and ([0-9]+) clay. "
            "Each geode robot costs ([0-9]+) ore and ([0-9]+) obsidian.");

    std::vector<blueprint> ret;
    std::string line;
    while (std::getline(is, line)) {
        std::smatch m;
        if (std::regex_match(line, m, pattern)) {
            blueprint bp{};
            bp.ore_robot_ore_cost = std::stol(m[1]);
            bp.clay_robot_ore_cost = std::stol(m[2]);
            bp.obsidian_robot_ore_cost = std::stol(m[3]);
            bp.obsidian_robot_clay_cost = std::stol(m[4]);
            bp.geode_robot_ore_cost = std::stol(m[5]);
            bp.geode_robot_obsidian_cost = std::stol(m[6]);
            ret.push_back(bp);
        } else {
            std::cerr << "malformed line: " << line << std::endl;
        }
    }
    return ret;
}

struct state_t {
    long time_left = 0;
    long ore = 0;
    long clay = 0;
    long obsidian = 0;
    long geodes = 0;
    long ore_robots = 0;
    long clay_robots = 0;
    long obsidian_robots = 0;
    long geode_robots = 0;
};

state_t advance(state_t const& state) {
    state_t new_state = state;
    --new_state.time_left;
    new_state.ore += new_state.ore_robots;
    new_state.clay += new_state.clay_robots;
    new_state.obsidian += new_state.obsidian_robots;
    new_state.geodes += new_state.geode_robots;
    return new_state;
}

state_t build_ore_robot(state_t const& state, blueprint const& bp) {
    state_t new_state = state;
    ++new_state.ore_robots;
    new_state.ore -= bp.ore_robot_ore_cost;
    return new_state;
}

state_t build_clay_robot(state_t const& state, blueprint const& bp) {
    state_t new_state = state;
    ++new_state.clay_robots;
    new_state.ore -= bp.clay_robot_ore_cost;
    return new_state;
}

state_t build_obsidian_robot(state_t const& state, blueprint const& bp) {
    state_t new_state = state;
    ++new_state.obsidian_robots;
    new_state.ore -= bp.obsidian_robot_ore_cost;
    new_state.clay -= bp.obsidian_robot_clay_cost;
    return new_state;
}

state_t build_geode_robot(state_t const& state, blueprint const& bp) {
    state_t new_state = state;
    ++new_state.geode_robots;
    new_state.ore -= bp.geode_robot_ore_cost;
    new_state.obsidian -= bp.geode_robot_obsidian_cost;
    return new_state;
}

long search(blueprint const& bp) {
    std::vector<state_t> stack = {{24, 0, 0, 0, 0, 1, 0, 0, 0}};
    long most_geodes = 0;

    while (!stack.empty()) {
        state_t const state = stack.back();
        stack.pop_back();

        if (state.geodes > most_geodes) {
            most_geodes = state.geodes;
        }
        if (state.time_left == 0) {
            continue;
        }

        if (state.ore >= bp.ore_robot_ore_cost && state.ore_robots < bp.max_ore_cost()) {
            stack.push_back(build_ore_robot(advance(state), bp));
        }

        if (state.ore >= bp.clay_robot_ore_cost && state.clay_robots < bp.obsidian_robot_clay_cost) {
            stack.push_back(build_clay_robot(advance(state), bp));
        }

        if (state.ore >= bp.obsidian_robot_ore_cost &&
            state.clay >= bp.obsidian_robot_clay_cost &&
            state.obsidian_robots < bp.geode_robot_obsidian_cost) {
            stack.push_back(build_obsidian_robot(advance(state), bp));
        }

        if (state.ore >= bp.geode_robot_ore_cost && state.obsidian >= bp.geode_robot_obsidian_cost) {
            stack.push_back(build_geode_robot(advance(state), bp));
        }

        stack.push_back(advance(state));

    }

    return most_geodes;
}

int main() {
    auto const input = parse_input(std::cin);

    long total_quality = 0;
    for (long i = 0; i < static_cast<long>(input.size()); ++i) {
        long const geodes = search(input[i]);
        total_quality += (i+1) * geodes;
    }
    std::cout << total_quality << "\n";
}
