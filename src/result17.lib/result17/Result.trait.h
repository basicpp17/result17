#pragma once
#include <type_traits> // std::true_type

namespace result17 {

// forward
template<class T, class E>
struct Result;

template<class R>
struct is_result : std::false_type {};

template<class T, class E>
struct is_result<Result<T, E>> : std::true_type {};

template<class R>
constexpr bool is_result_v = is_result<R>::value;

} // namespace result17
