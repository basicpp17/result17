#pragma once
#include <utility> // std::forward

namespace result17 {

template<class T>
struct Ok {
    T value{};
};
template<>
struct Ok<void> {};

template<class T>
Ok(T)->Ok<T>;

template<class T>
constexpr inline auto ok(T&& t) {
    return Ok<T>{std::forward<T>(t)};
}

} // namespace result17
