#pragma once
#include <utility> // std::forward

namespace result17 {

template<class E>
struct Error {
    E reason{};
};

template<class E>
Error(E)->Error<E>;

template<class E>
constexpr inline auto error(E&& e) {
    return Error<E>{std::forward<E>(e)};
}

} // namespace result17
