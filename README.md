# C++ Result

A Rust inspired `result<V,E>` type for C++17.

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

see `Result.test.cpp` for a working example.


## Requirements

You will need a C++17 compiler.

* [x] GCC 8.x
* [x] Clang 8.x + libc++
* [x] MSVC2017/2019
* [ ] googletest is optional to run the tests

## Links

* [github/oktal/result](https://github.com/oktal/result) a similar result type for C++14
* [doc.rust-lang/std/result](https://doc.rust-lang.org/std/result/) the rust result type documentation

## Status

Even though this seems to work, it is only a proof of concept.

* [x] it works
* [ ] `std::variant` is unusable slow for this kind of use case.
* [ ] `std::reference_wrapper` does not allow comparisons.
