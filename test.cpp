#include "result.h"

#include <fstream>
#include <sstream>
using std::ifstream;
using std::stringstream;
using namespace std::literals;

[[nodiscard]] auto openFile(const string &filename) -> Result<ifstream, string> {
    ifstream s(filename);

    if (!s.is_open()) return error("could not open"s);
    return ok(move(s));
}

#include <vector>

[[nodiscard]] auto splitLines(stringstream &in) -> std::vector<string> {
    auto result = std::vector<string>{};
    auto line = string{};
    while (std::getline(in, line)) result.emplace_back(line);
    return result;
}

#include <algorithm>
#include <iostream>
#include <string>

int main() {
    auto f = openFile("hello.txt") //
                 .orMap([](auto &&) { return openFile(__FILE__); })
                 .andMap([](auto &&in) -> Result<stringstream, string> {
                     stringstream out;
                     out << in.rdbuf();
                     if (out.str().empty()) return error("empty file"s);
                     return ok(move(out));
                 });
    auto c = f.andMap(&splitLines);

    c.andMap([](auto &in) {
        auto mid = std::stable_partition(
            in.begin(), in.end(), [](const auto &l) { return !l.empty() && l.front() != '#' && l.front() != '\r'; });
        in.erase(mid, in.end());
    });

    auto o = c.orMap([](auto) { return std::vector<string>{}; });
    for (auto &l : o.unwrap()) std::cout << l << '\n';
}
