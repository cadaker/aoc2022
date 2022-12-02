#include <iostream>
#include <vector>
#include <numeric>

enum rps {
    rock,
    paper,
    scissors,
};

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
    switch (shape) {
        case rock: return 1;
        case paper: return 2;
        case scissors: return 3;
        default: abort();
    }
}

rps winning_option(rps shape) {
    switch (shape) {
        case rock: return paper;
        case paper: return scissors;
        case scissors: return rock;
        default: abort();
    }
}

rps losing_option(rps shape) {
    switch (shape) {
        case rock: return scissors;
        case paper: return rock;
        case scissors: return paper;
        default: abort();
    }
}

int outcome_score(rps opponent, rps you) {
    if (you == winning_option(opponent)) {
        return 6;
    } else if (opponent == winning_option(you)) {
        return 0;
    } else {
        return 3;
    }
}

rps option_as_rps(option op) {
    switch (op) {
        case option_x: return rock;
        case option_y: return paper;
        case option_z: return scissors;
        default: abort();
    }
}

int score1(std::pair<rps, option> const& round) {
    return shape_score(option_as_rps(round.second)) + outcome_score(round.first, option_as_rps(round.second));
}

rps option_as_outcome(rps opponent, option outcome) {
    switch (outcome) {
        case option_x: return losing_option(opponent);
        case option_y: return opponent;
        case option_z: return winning_option(opponent);
        default: abort();
    }
}

int score2(std::pair<rps, option> const& round) {
    rps const choice = option_as_outcome(round.first, round.second);
    return shape_score(choice) + outcome_score(round.first, choice);
}

int main() {
    auto const input = parse_input(std::cin);

    std::cout << std::accumulate(input.begin(), input.end(), 0L, [](long acc, auto const& round) {
        return acc + score1(round);
    }) << "\n";
    std::cout << std::accumulate(input.begin(), input.end(), 0L, [](long acc, auto const& round) {
        return acc + score2(round);
    }) << "\n";
}
