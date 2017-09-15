#include "result.h"

#include <fstream>
#include <sstream>
using std::ifstream;
using std::stringstream;
using namespace std::literals;

[[nodiscard]]
auto OpenFile(const string& filename) -> Result<ifstream, string> {
    ifstream s(filename);

    if (!s.is_open()) return Error{"could not open"s};
    return Ok{ move(s) };
}

[[nodiscard]]
auto ReadAll(ifstream& s) -> Result<stringstream, string> {
    stringstream ss;
    ss << s.rdbuf();
    return Ok{ move(ss) };
}

void test() {
  OpenFile("hello.txt"s)
  .and_map(&ReadAll)
  .ok();
}
