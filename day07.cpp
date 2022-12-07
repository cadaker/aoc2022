#include <iostream>
#include <vector>
#include <memory>
#include <regex>
#include <unordered_map>

struct dir_t {
    std::unordered_map<std::string, long> files;
    std::unordered_map<std::string, std::unique_ptr<dir_t>> subdirs;
};

std::unique_ptr<dir_t> parse_input(std::istream& is) {
    auto root = std::make_unique<dir_t>();
    std::vector<dir_t*> dirstack = {root.get()};

    std::regex const chdir(R"(^\$ cd (.*)$)");
    std::regex const dirlist(R"(^dir (.*)$)");
    std::regex const filelist(R"(^(\d+) (.*)$)");
    std::smatch m;

    std::string line;
    while (std::getline(is, line)) {
        if (line == "$ cd /") {
            // Do nothing
        } else if (line == "$ cd ..") {
            dirstack.pop_back();
        } else if (std::regex_match(line, m, chdir)) {
            dir_t* newdir = (dirstack.back()->subdirs[m[1]] = std::make_unique<dir_t>()).get();
            dirstack.push_back(newdir);
        } else if (line == "$ ls") {
            // Do nothing
        } else if (std::regex_match(line, m, dirlist)) {
            // Do nothing
        } else if (std::regex_match(line, m, filelist)) {
            dirstack.back()->files[m[2]] = std::stol(m[1]);
        } else {
            std::cerr << "Unknown input line: " << line << "\n";
        }
    }
    return root;
}

long find_small_dirs(dir_t const& dir, long& total, long limit) {
    long dir_size = 0;
    for (auto const& f : dir.files) {
        dir_size += f.second;
    }
    for (auto const& d : dir.subdirs) {
        long subdir_size = find_small_dirs(*d.second, total, limit);
        dir_size += subdir_size;
    }
    if (dir_size <= limit) {
        total += dir_size;
    }
    return dir_size;
}

int main() {
    auto input = parse_input(std::cin);

    long small_dir_total = 0;
    find_small_dirs(*input, small_dir_total, 100000);
    std::cout << small_dir_total << "\n";
}
