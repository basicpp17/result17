# C++ Result

A Rust inspired `result<V,E>` type for modern C++.

```cpp
template<typename V, typename E>
struct result;
```

basically a sophisticated wrapper for:

```cpp
std::variant< ok<V>, error<E> >
```

## Usage synopsis

```cpp
auto fn() -> Result<std::ifstream, string>;
auto splitLines(std::stringstream&) -> std::vector<string>;

int main() {
    auto f = openFile("hello.txt")
        .orMap([](auto&&) { return openFile("fallback.txt"); }) // invoked only on error
        .andMap([](auto&& in) -> Result<std::stringstream, string> { // invoked only if successful
            std::stringstream out;
            out << in.rdbuf();
            if (out.str().empty()) return error("empty file"s);
            return ok(move(out));
        })
        .andMap(&splitLines) // adds no errors
        .orMap([](auto) -> Result<std::vector<string>, string> { 
            return ok(std::vector<string>{}); 
        });
    for (auto &l : f.unwrap()) std::cout << l << '\n'; // we are sure we have a value
}
```

see `test.cpp` for a working example.

## Requirements

You will need a C++17 compiler.

* [x] GCC 7.x works
* [x] Clang 5.x + libc++ works
* [x] VS2017 works

## Links

* [github/oktal/result](https://github.com/oktal/result) a similar result type for C++14
* [doc.rust-lang/std/result](https://doc.rust-lang.org/std/result/) the rust result type documentation
