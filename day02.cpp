#include "algorithm.hpp"
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>

enum rps {
    rock = 0,
    paper = 1,
    scissors = 2,
};

rps winner_against(rps choice) {
    return static_cast<rps>((choice + 1) % 3);
}

rps loser_against(rps choice) {
    return static_cast<rps>((choice + 2) % 3);
}

enum option {
    option_x,
    option_y,
    option_z,
};

std::vector<std::pair<rps, option>> parse_input(std::istream& is) {
    std::vector<std::pair<rps, option>> ret;
    std::string line;
    while (std::getline(is, line)) {
        rps const first = line.at(0) == 'A' ? rock : line.at(0) == 'B' ? paper : scissors;
        option const second = line.at(2) == 'X' ? option_x : line.at(2) == 'Y' ? option_y : option_z;
        ret.emplace_back(first, second);
    }
    return ret;
}

int shape_score(rps shape) {
    return shape + 1;
}

int outcome_score(rps opponent, rps you) {
    if (you == winner_against(opponent)) {
        return 6;
    } else if (opponent == winner_against(you)) {
        return 0;
    } else {
        return 3;
    }
}

using round = std::pair<rps, rps>;

rps option_as_rps(option op) {
    switch (op) {
        case option_x: return rock;
        case option_y: return paper;
        case option_z: return scissors;
        default: abort();
    }
}

round options_as_rps(std::pair<rps, option> const& p) {
    return {p.first, option_as_rps(p.second)};
}

int score_round(round const& r) {
    return shape_score(r.second) + outcome_score(r.first, r.second);
}

rps option_as_outcome(rps opponent, option outcome) {
    switch (outcome) {
        case option_x: return loser_against(opponent);
        case option_y: return opponent;
        case option_z: return winner_against(opponent);
        default: abort();
    }
}

round options_as_outcome(std::pair<rps, option> const& p) {
    return {p.first, option_as_outcome(p.first, p.second)};
}

int main() {
    auto const input = parse_input(std::cin);

    std::vector<round> rounds1;
    std::transform(input.begin(), input.end(), std::back_inserter(rounds1), &options_as_rps);

    std::vector<round> rounds2;
    std::transform(input.begin(), input.end(), std::back_inserter(rounds2), &options_as_outcome);

    std::cout << accumulate_map(rounds1.begin(), rounds1.end(), 0L, score_round) << "\n";
    std::cout << accumulate_map(rounds2.begin(), rounds2.end(), 0L, score_round) << "\n";
}
