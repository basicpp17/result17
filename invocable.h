#pragma once

#include <type_traits>

/// simplified workaround for clang 5 & libc++ miss this type trait

namespace details {

template<class Fn>
struct invocable_test {

    template<typename...>
    struct success : std::true_type {};

    template<typename... Args>
    auto operator()(Args &&... args) const -> success<decltype(std::declval<Fn>()(static_cast<Args &&>(args)...))>;

    auto operator()(...) const -> std::false_type;
};

template<class pObj, class pRet, class... pArgs>
struct invocable_test<pRet (pObj::*)(pArgs...)> {
    using Fn = pRet (pObj::*)(pArgs...);

    template<typename...>
    struct success : std::true_type {};

    template<typename Obj, typename... Args>
    auto operator()(Obj &&o, Args &&... args) const
        -> success<decltype((o.*std::declval<Fn>())(static_cast<Args &&>(args)...))>;

    auto operator()(...) const -> std::false_type;
};
} // namespace details

template<class Fn, class... Args>
struct is_invocable : decltype(details::invocable_test<Fn>()(std::declval<Args>()...)) {};

template<class Fn, class... Args>
constexpr bool is_invocable_v = is_invocable<Fn, Args...>::value;
