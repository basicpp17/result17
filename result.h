#include "invocable.h"
#include "overloaded.h"

#include <exception>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

using std::move;
using std::optional;
using std::string;
using std::string_view;
using std::variant;
using namespace std::literals;

template<class T>
struct always_false : std::false_type {};

template<class T>
struct Ok {
    T value;
};
template<>
struct Ok<void> {};

template<class E>
struct Error {
    E reason;
};

// template deduction guides
// template<class T> Ok(T) -> Ok<T>;
// template<class E> Error(E) -> Error<E>;
template<class T>
constexpr inline auto ok(T &&t) {
    return Ok<T>{std::forward<T>(t)};
}
template<class E>
constexpr inline auto error(E &&e) {
    return Error<E>{std::forward<E>(e)};
}

template<class R>
struct is_result : std::false_type {};

template<class R>
constexpr bool is_result_v = is_result<R>::value;

template<bool B = false, class... T>
void debug_helper() {
    struct X {
        static_assert(B, "debug");
    };
}

template<class T, class E>
struct Result {
    using OkT = Ok<T>;
    using ErrorE = Error<E>;

    Result() = delete;
    Result(const Result &) = default;
    Result &operator=(const Result &) & = default;
    Result(Result &&) = default;
    Result &operator=(Result &&) & = default;

    Result(const OkT &ok)
        : m(ok) {}
    Result(OkT &&ok)
        : m(move(ok)) {}

    Result(const ErrorE &error)
        : m(error) {}
    Result(ErrorE &&error)
        : m(move(error)) {}

    bool operator==(const Result &o) const { return m == o.m; }
    bool operator!=(const Result &o) const { return m != o.m; }
    bool operator==(const OkT &o) const { return m == o; }
    bool operator==(const ErrorE &o) const { return m == o; }

    bool is_ok() const { return std::holds_alternative<OkT>(m); }
    bool is_error() const { return std::holds_alternative<ErrorE>(m); }

    auto ok() const & -> optional<std::reference_wrapper<const T>> {
        using R = optional<const T &>;
        return std::visit(
            overloaded([](const OkT &o) -> R { return std::cref(o.value); }, [](const ErrorE &) -> R { return {}; }),
            m);
    }
    auto ok() & -> optional<std::reference_wrapper<T>> {
        using R = optional<std::reference_wrapper<T>>;
        return std::visit(overloaded([](OkT &o) -> R { return std::ref(o.value); }, //
                                     [](ErrorE &) -> R { return {}; }),
                          m);
    }
    auto ok() && -> optional<T> {
        using R = optional<T>;
        return std::visit(overloaded([](OkT &o) -> R { return std::move(o.value); }, //
                                     [](ErrorE &) -> R { return {}; }),
                          std::move(m));
    }

    // TODO: auto err() const & -> optional<std::reference_wrapper<const E>>;
    // TODO: auto err() & -> optional<std::reference_wrapper<E>>;
    // TODO: auto err() && -> optional<E>;

    /// evolve value unless error
    template<class F>
    [[nodiscard]] auto andMap(F &&f) const & {
        if constexpr (is_invocable_v<F, const T &>) {
            using f_ret = decltype(f(std::declval<const T &>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(overloaded([](const ErrorE &) {}, //
                                             [ff = std::forward<F>(f)](const OkT &o) { std::invoke(ff, o.value); }),
                                  m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
                return std::visit(
                    overloaded([](const ErrorE &e) -> R { return e; }, //
                               [ff = std::forward<F>(f)](const OkT &o)->R { return std::invoke(ff, o.value); }),
                    m);
            }
            else {
                using R = Result<f_ret, E>;
                return std::visit(
                    overloaded([](const ErrorE &e) -> R { return e; }, //
                               [ff = std::forward<F>(f)](const OkT &o)->R { return ::ok(std::invoke(ff, o.value)); }),
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(overloaded([](const ErrorE &e) -> R { return e; }, //
                                         [ff = std::forward<F>(f)](const OkT &)->R { return ff; }),
                              m);
        }
        else {
            using R = Result<F, E>;
            return std::visit(overloaded([](const ErrorE &e) -> R { return e; }, //
                                         [ff = std::forward<F>(f)](const OkT &)->R { return ::ok(ff); }),
                              m);
        }
    }
    template<class F>
    [[nodiscard]] auto andMap(F &&f) & {
        if constexpr (is_invocable_v<F, T &>) {
            using f_ret = decltype(f(std::declval<T &>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(overloaded([](ErrorE &) {}, //
                                             [ff = std::forward<F>(f)](OkT & o) { std::invoke(ff, o.value); }),
                                  m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
                return std::visit(
                    overloaded([](ErrorE &e) -> R { return e; }, //
                               [ff = std::forward<F>(f)](OkT & o)->R { return std::invoke(ff, o.value); }),
                    m);
            }
            else {
                using R = Result<f_ret, E>;
                return std::visit(
                    overloaded([](ErrorE &e) -> R { return e; }, //
                               [ff = std::forward<F>(f)](OkT & o)->R { return ::ok(std::invoke(ff, o.value)); }),
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(overloaded([](ErrorE &e) -> R { return e; }, //
                                         [ff = std::forward<F>(f)](OkT &)->R { return ff; }),
                              m);
        }
        else {
            using R = Result<F, E>;
            return std::visit(overloaded([](ErrorE &e) -> R { return e; }, //
                                         [ff = std::forward<F>(f)](OkT &)->R { return ::ok(ff); }),
                              m);
        }
    }
    template<class F>
    [[nodiscard]] auto andMap(F &&f) && {
        using f_ret = decltype(std::declval<F>()(std::declval<T &>()));
        if constexpr (is_result_v<f_ret>) {
            using R = f_ret;
            static_assert(std::is_same_v<typename f_ret::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(
                overloaded([](ErrorE &&e) -> R { return std::move(e); }, //
                           [ff = std::forward<F>(f)](OkT && o)->R { return std::invoke(ff, std::move(o.value)); }),
                std::move(m));
        }
        else {
            using R = Result<f_ret, E>;
            return std::visit(overloaded([](ErrorE &&e) -> R { return std::move(e); }, //
                                         [ff = std::forward<F>(f)](OkT && o)->R {
                                             return ::ok(std::invoke(ff, std::move(o.value)));
                                         }),
                              std::move(m));
        }
    }

    /// evolve error unless valid
    template<class F>
    [[nodiscard]] auto orMap(F &&f) const & {
        if constexpr (is_invocable_v<F, const E &>) {
            using f_ret = decltype(f(std::declval<const E &>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(overloaded([](const OkT &) {}, //
                                             [ff = std::forward<F>(f)](const ErrorE &e) { std::invoke(ff, e.reason); }),
                                  m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
                return std::visit(
                    overloaded([](const OkT &o) -> R { return o; }, //
                               [ff = std::forward<F>(f)](const ErrorE &e)->R { return std::invoke(ff, e.reason); }),
                    m);
            }
            else {
                using R = Result<T, f_ret>;
                return std::visit(overloaded([](const OkT &o) -> R { return o; }, //
                                             [ff = std::forward<F>(f)](const ErrorE &e)->R {
                                                 return ::ok(std::invoke(ff, e.reason));
                                             }),
                                  m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(overloaded([](const OkT &o) -> R { return o; }, //
                                         [ff = std::forward<F>(f)](const ErrorE &)->R { return ff; }),
                              m);
        }
        else {
            using R = Result<T, E>;
            return std::visit(overloaded([](const OkT &o) -> R { return o; }, //
                                         [ff = std::forward<F>(f)](const ErrorE &)->R { return ::ok(ff); }),
                              m);
        }
    }
    template<class F>
    [[nodiscard]] auto orMap(F &&f) & {
        if constexpr (is_invocable_v<F, E &>) {
            using f_ret = decltype(f(std::declval<E &>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(overloaded([](OkT &) {}, //
                                             [ff = std::forward<F>(f)](ErrorE & e) { std::invoke(ff, e.reason); }),
                                  m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
                return std::visit(
                    overloaded([](OkT &o) -> R { return o; }, //
                               [ff = std::forward<F>(f)](ErrorE & e)->R { return std::invoke(ff, e.reason); }),
                    m);
            }
            else {
                using R = Result<T, f_ret>;
                return std::visit(
                    overloaded([](OkT &o) -> R { return o; }, //
                               [ff = std::forward<F>(f)](ErrorE & e)->R { return ::ok(std::invoke(ff, e.reason)); }),
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(overloaded([](OkT &o) -> R { return o; }, //
                                         [ff = std::forward<F>(f)](ErrorE &)->R { return ff; }),
                              m);
        }
        else {
            using R = Result<T, F>;
            return std::visit(overloaded([](OkT &o) -> R { return o; }, //
                                         [ff = std::forward<F>(f)](ErrorE &)->R { return ::ok(ff); }),
                              m);
        }
    }
    template<class F>
    [[nodiscard]] auto orMap(F &&f) && {
        using f_ret = decltype(std::declval<F>()(std::declval<E &>()));
        if constexpr (is_result_v<f_ret>) {
            using R = f_ret;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(
                overloaded([](OkT &&o) -> R { return std::move(o); }, //
                           [ff = std::forward<F>(f)](ErrorE && e)->R { return std::invoke(ff, std::move(e.reason)); }),
                std::move(m));
        }
        else {
            using R = Result<T, f_ret>;
            return std::visit(overloaded([](OkT &&o) -> R { return std::move(o); }, //
                                         [ff = std::forward<F>(f)](ErrorE && e)->R {
                                             return ::ok(std::invoke(ff, std::move(e.reason)));
                                         }),
                              std::move(m));
        }
    }

    /// get valid value or raise exception
    auto unwrap() const & -> const T & {
        if (is_error()) throw std::bad_variant_access();
        return std::get<OkT>(m).value;
    }

    /// get error or rais exception
    auto unwrapError() const & -> const E & {
        if (is_ok()) throw std::bad_variant_access();
        return std::get<ErrorE>(m).value;
    }

    auto unwrapOr(const T &t) const & -> const T & {
        return std::visit(overloaded([&](const ErrorE &) -> const T & { return t; },
                                     [](const OkT &o) -> const T & { return o.value; }),
                          m);
    }
    template<class F>
    auto unwrapOrMap(F &&f) const & -> T {
        return std::visit(
            overloaded([ff = std::forward<F>(f)](const ErrorE &e)->T { return std::invoke(ff, e.reason); },
                       [](const OkT &o) -> T { return o.value; }),
            m);
    }

private:
    std::variant<OkT, ErrorE> m;
};

template<class T, class E>
struct is_result<Result<T, E>> : std::true_type {};
