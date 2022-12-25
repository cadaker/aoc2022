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

state_t advance(state_t const& state, long time = 1) {
    state_t new_state = state;
    new_state.time_left -= time;
    new_state.ore += new_state.ore_robots * time;
    new_state.clay += new_state.clay_robots * time;
    new_state.obsidian += new_state.obsidian_robots * time;
    new_state.geodes += new_state.geode_robots * time;
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

long until(long owned, long needed, long per_turn) {
    if (owned >= needed) {
        return 0;
    } else if (per_turn == 0) {
        return 100000; // Big, but not big enough to risk overflows.
    } else {
        return ((needed - owned) + per_turn - 1) / per_turn;
    }
}

long time_until_ore(state_t const& state, long ore) {
    return until(state.ore, ore, state.ore_robots);
}

long time_until_clay(state_t const& state, long clay) {
    return until(state.clay, clay, state.clay_robots);
}

long time_until_obsidian(state_t const& state, long obsidian) {
    return until(state.obsidian, obsidian, state.obsidian_robots);
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

        // Build geode bot
        long const geode_time = std::max(
                time_until_ore(state, bp.geode_robot_ore_cost),
                time_until_obsidian(state, bp.geode_robot_obsidian_cost));
        if (state.time_left > geode_time) {
            stack.push_back(build_geode_robot(advance(state, geode_time + 1), bp));
        }

        long const obsidian_time = std::max(
                time_until_clay(state, bp.obsidian_robot_clay_cost),
                time_until_ore(state, bp.obsidian_robot_ore_cost));
        if (state.time_left > obsidian_time && state.obsidian_robots < bp.geode_robot_obsidian_cost) {
            stack.push_back(build_obsidian_robot(advance(state, obsidian_time + 1), bp));
        }

        long const clay_time = time_until_ore(state, bp.clay_robot_ore_cost);
        if (state.time_left > clay_time && state.clay_robots < bp.obsidian_robot_clay_cost) {
            stack.push_back(build_clay_robot(advance(state, clay_time + 1), bp));
        }

        long const ore_time = time_until_ore(state, bp.ore_robot_ore_cost);
        if (state.time_left > ore_time && state.ore_robots < bp.max_ore_cost()) {
            stack.push_back(build_ore_robot(advance(state, ore_time + 1), bp));
        }

        stack.push_back(advance(state, state.time_left));
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
