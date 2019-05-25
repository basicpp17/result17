#pragma once
#include "Error.h"
#include "Ok.h"
#include "Overloaded.h"
#include "Result.trait.h"

#include <optional> // std::optional
#include <type_traits>
#include <variant> // std::variant

namespace result17 {

using std::optional;

template<class T, class E>
struct Result {
    using OkT = Ok<T>;
    using ErrorE = Error<E>;
    using M = std::variant<OkT, ErrorE>;

    Result() = delete;
    Result(const Result&) = default;
    Result& operator=(const Result&) = default;
    Result(Result&&) noexcept = default;
    Result& operator=(Result&&) noexcept = default;
    ~Result() = default;

    /*implicit*/ Result(const OkT& ok)
        : m(ok) {}
    /*implicit*/ Result(OkT&& ok)
        : m(std::move(ok)) {}

    /*implicit*/ Result(const ErrorE& error)
        : m(error) {}
    /*implicit*/ Result(ErrorE&& error)
        : m(std::move(error)) {}

    bool is_ok() const { return std::holds_alternative<OkT>(m); }
    bool is_error() const { return std::holds_alternative<ErrorE>(m); }

    auto ok() const & noexcept -> optional<std::reference_wrapper<const T>> {
        using R = optional<std::reference_wrapper<const T>>;
        return std::visit(
            Overloaded{
                [](const OkT& o) -> R { return std::cref(o.value); }, //
                [](const ErrorE&) -> R { return {}; } //
            },
            m);
    }
    auto ok() & noexcept -> optional<std::reference_wrapper<T>> {
        using R = optional<std::reference_wrapper<T>>;
        return std::visit(
            Overloaded{
                [](OkT& o) -> R { return std::ref(o.value); }, //
                [](ErrorE&) -> R { return {}; } //
            },
            m);
    }
    auto ok() && -> optional<T> {
        using R = optional<T>;
        return std::visit(
            Overloaded{
                [](OkT&& o) -> R { return std::move(o.value); }, //
                [](ErrorE &&) -> R { return {}; } //
            },
            std::move(m));
    }

    auto error() const & noexcept -> optional<std::reference_wrapper<const E>> {
        using R = optional<std::reference_wrapper<const E>>;
        return std::visit(
            Overloaded{
                [](const OkT&) -> R { return {}; }, //
                [](const ErrorE& e) -> R { return std::cref(e.reason); } //
            },
            m);
    }
    auto error() & noexcept -> optional<std::reference_wrapper<E>> {
        using R = optional<std::reference_wrapper<E>>;
        return std::visit(
            Overloaded{
                [](OkT&) -> R { return {}; }, //
                [](ErrorE& e) -> R { return std::ref(e.reason); } //
            },
            m);
    }
    auto error() && -> optional<E> {
        using R = optional<E>;
        return std::visit(
            Overloaded{
                [](OkT &&) -> R { return {}; }, //
                [](ErrorE&& e) -> R { return std::move(e.reason); } //
            },
            std::move(m));
    }

    /// evolve value unless error
    template<class F>
    [[nodiscard]] auto andMap(F&& f) const& {
        if constexpr (std::is_invocable_v<F, const T&>) {
            using f_ret = decltype(f(std::declval<const T&>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(
                    Overloaded{
                        [](const ErrorE&) {}, //
                        [ff = std::forward<F>(f)](const OkT& o) { std::invoke(ff, o.value); } //
                    },
                    m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
                return std::visit(
                    Overloaded{
                        [](const ErrorE& e) -> R { return e; }, //
                        [ff = std::forward<F>(f)](const OkT& o) -> R { return std::invoke(ff, o.value); } //
                    },
                    m);
            }
            else {
                using R = Result<f_ret, E>;
                return std::visit(
                    Overloaded{
                        [](const ErrorE& e) -> R { return e; }, //
                        [ff = std::forward<F>(f)](const OkT& o) -> R { return Ok{std::invoke(ff, o.value)}; } //
                    },
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(
                Overloaded{
                    [](const ErrorE& e) -> R { return e; }, //
                    [ff = std::forward<F>(f)](const OkT&) -> R { return ff; } //
                },
                m);
        }
        else {
            using R = Result<F, E>;
            return std::visit(
                Overloaded{
                    [](const ErrorE& e) -> R { return e; }, //
                    [ff = std::forward<F>(f)](const OkT&) -> R { return Ok{ff}; } //
                },
                m);
        }
    }
    template<class F>
    auto andMap(F&& f) & {
        if constexpr (std::is_invocable_v<F, T&>) {
            using f_ret = decltype(f(std::declval<T&>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(
                    Overloaded{
                        [](ErrorE&) {}, //
                        [ff = std::forward<F>(f)](OkT& o) { std::invoke(ff, o.value); } //
                    },
                    m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
                return std::visit(
                    Overloaded{
                        [](ErrorE& e) -> R { return e; }, //
                        [ff = std::forward<F>(f)](OkT& o) -> R { return std::invoke(ff, o.value); } //
                    },
                    m);
            }
            else {
                using R = Result<f_ret, E>;
                return std::visit(
                    Overloaded{
                        [](ErrorE& e) -> R { return e; }, //
                        [ff = std::forward<F>(f)](OkT& o) -> R { return Ok{std::invoke(ff, o.value)}; } //
                    },
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(
                Overloaded{
                    [](ErrorE& e) -> R { return e; }, //
                    [ff = std::forward<F>(f)](OkT&) -> R { return ff; } //
                },
                m);
        }
        else {
            using R = Result<F, E>;
            return std::visit(
                Overloaded{[](ErrorE& e) -> R { return e; }, //
                           [ff = std::forward<F>(f)](OkT&) -> R { return Ok{ff}; }},
                m);
        }
    }
    template<class F>
    [[nodiscard]] auto andMap(F&& f) && {
        using f_ret = decltype(std::declval<F>()(std::declval<T&>()));
        if constexpr (is_result_v<f_ret>) {
            using R = f_ret;
            static_assert(std::is_same_v<typename f_ret::ErrorE, ErrorE>, "error type has to stay!");
            return std::visit(
                Overloaded{
                    [](ErrorE&& e) -> R { return std::move(e); }, //
                    [ff = std::forward<F>(f)](OkT&& o) -> R { return std::invoke(ff, std::move(o.value)); } //
                },
                std::move(m));
        }
        else {
            using R = Result<f_ret, E>;
            return std::visit(
                Overloaded{
                    [](ErrorE&& e) -> R { return std::move(e); }, //
                    [ff = std::forward<F>(f)](OkT&& o) -> R { return Ok{std::invoke(ff, std::move(o.value))}; } //
                },
                std::move(m));
        }
    }

    /// evolve error unless valid
    template<class F>
    [[nodiscard]] auto orMap(F&& f) const& {
        if constexpr (std::is_invocable_v<F, const E&>) {
            using f_ret = decltype(f(std::declval<const E&>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(
                    Overloaded{
                        [](const OkT&) {}, //
                        [ff = std::forward<F>(f)](const ErrorE& e) { std::invoke(ff, e.reason); } //
                    },
                    m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
                return std::visit(
                    Overloaded{
                        [](const OkT& o) -> R { return o; }, //
                        [ff = std::forward<F>(f)](const ErrorE& e) -> R { return std::invoke(ff, e.reason); } //
                    },
                    m);
            }
            else {
                using R = Result<T, f_ret>;
                return std::visit(
                    Overloaded{
                        [](const OkT& o) -> R { return o; }, //
                        [ff = std::forward<F>(f)](const ErrorE& e) -> R { return Ok{std::invoke(ff, e.reason)}; } //
                    },
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(
                Overloaded{
                    [](const OkT& o) -> R { return o; }, //
                    [ff = std::forward<F>(f)](const ErrorE&) -> R { return ff; } //
                },
                m);
        }
        else {
            using R = Result<T, F>;
            return std::visit(
                Overloaded{
                    [](const OkT& o) -> R { return o; }, //
                    [ff = std::forward<F>(f)](const ErrorE&) -> R { return Ok{ff}; } //
                },
                m);
        }
    }
    template<class F>
    [[nodiscard]] auto orMap(F&& f) & {
        if constexpr (std::is_invocable_v<F, E&>) {
            using f_ret = decltype(f(std::declval<E&>()));
            if constexpr (std::is_same_v<void, f_ret>) {
                return std::visit(
                    Overloaded{
                        [](OkT&) {}, //
                        [ff = std::forward<F>(f)](ErrorE& e) { std::invoke(ff, e.reason); } //
                    },
                    m);
            }
            else if constexpr (is_result_v<f_ret>) {
                using R = f_ret;
                static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
                return std::visit(
                    Overloaded{
                        [](OkT& o) -> R { return o; }, //
                        [ff = std::forward<F>(f)](ErrorE& e) -> R { return std::invoke(ff, e.reason); } //
                    },
                    m);
            }
            else {
                using R = Result<T, f_ret>;
                return std::visit(
                    Overloaded{
                        [](OkT& o) -> R { return o; }, //
                        [ff = std::forward<F>(f)](ErrorE& e) -> R { return Ok{std::invoke(ff, e.reason)}; } //
                    },
                    m);
            }
        }
        else if constexpr (is_result_v<F>) {
            using R = F;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(
                Overloaded{
                    [](OkT& o) -> R { return o; }, //
                    [ff = std::forward<F>(f)](ErrorE&) -> R { return ff; } //
                },
                m);
        }
        else {
            using R = Result<T, F>;
            return std::visit(
                Overloaded{
                    [](OkT& o) -> R { return o; }, //
                    [ff = std::forward<F>(f)](ErrorE&) -> R { return Ok{ff}; } //
                },
                m);
        }
    }
    template<class F>
    [[nodiscard]] auto orMap(F&& f) && {
        using f_ret = decltype(std::declval<F>()(std::declval<E&>()));
        if constexpr (is_result_v<f_ret>) {
            using R = f_ret;
            static_assert(std::is_same_v<typename R::OkT, OkT>, "ok type has to stay!");
            return std::visit(
                Overloaded{
                    [](OkT&& o) -> R { return std::move(o); }, //
                    [ff = std::forward<F>(f)](ErrorE&& e) -> R { return std::invoke(ff, std::move(e.reason)); } //
                },
                std::move(m));
        }
        else {
            using R = Result<T, f_ret>;
            return std::visit(
                Overloaded{
                    [](OkT&& o) -> R { return std::move(o); }, //
                    [ff = std::forward<F>(f)](ErrorE&& e) -> R { return Ok{std::invoke(ff, std::move(e.reason))}; } //
                },
                std::move(m));
        }
    }

    /// get valid value or raise exception
    auto unwrap() const& -> const T& {
        if (is_error()) throw std::bad_variant_access();
        return std::get<OkT>(m).value;
    }

    /// get error or rais exception
    auto unwrapError() const& -> const E& {
        if (is_ok()) throw std::bad_variant_access();
        return std::get<ErrorE>(m).value;
    }

    auto unwrapOr(const T& t) const& -> const T& {
        return std::visit(
            Overloaded{
                [&](const ErrorE&) -> const T& { return t; }, //
                [](const OkT& o) -> const T& { return o.value; } //
            },
            m);
    }

    template<class F>
    auto unwrapOrMap(F&& f) const& -> T {
        return std::visit(
            Overloaded{
                [ff = std::forward<F>(f)](const ErrorE& e) -> T { return std::invoke(ff, e.reason); },
                [](const OkT& o) -> T { return o.value; } //
            },
            m);
    }

    M m;
};

} // namespace result17
