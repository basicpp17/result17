# C++ Result

A rust like Result type for modern C++.

```cpp
template<typename V, typename E>
struct result;
```

basically a wrapper for:

```cpp
std::variant< ok<V>, error<E> >
```

## requirements

You will need a C++17 compiler.
* [x] GCC 7.x works
* [x] Clang 5.x + libc++ works
* [ ] VS2017 *NOT* works

## links

* [github/oktal/result](https://github.com/oktal/result) a similar result type for C++14
* [doc.rust-lang/std/result](https://doc.rust-lang.org/std/result/) the rust result type documentation
