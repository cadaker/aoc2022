#include <iostream>
#include <vector>
#include <numeric>

enum rps {
    rock,
    paper,
    scissors,
};

std::vector<std::pair<rps, rps>> parse_input(std::istream& is) {
    std::vector<std::pair<rps, rps>> ret;
    std::string line;
    while (std::getline(is, line)) {
        rps const first = line.at(0) == 'A' ? rock : line.at(0) == 'B' ? paper : scissors;
        rps const second = line.at(2) == 'X' ? rock : line.at(2) == 'Y' ? paper : scissors;
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

int outcome_score(rps opponent, rps you) {
    switch (opponent) {
        case rock: return you == paper ? 6 : you == rock ? 3 : 0;
        case paper: return you == scissors ? 6 : you == paper ? 3 : 0;
        case scissors: return you == rock ? 6 : you == scissors ? 3 : 0;
        default: abort();
    }
}

int score(std::pair<rps, rps> const& round) {
    return shape_score(round.second) + outcome_score(round.first, round.second);
}

int main() {
    auto const input = parse_input(std::cin);

    std::cout << std::accumulate(input.begin(), input.end(), 0L, [](long acc, auto const& round) {
        return acc + score(round);
    }) << "\n";
}