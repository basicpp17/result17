#include "return_type.h"
#include "overloaded.h"

#include <variant>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <functional>

using std::variant;
using std::optional;
using std::string; 
using std::string_view;
using std::move;
using namespace std::literals;

template<class T> struct always_false : std::false_type {};

template<class T>
struct Ok {
    T value;
};
template<> struct Ok<void> {};

template<class E>
struct Error {
    E reason;
};

// template deduction guides
template<class T> Ok(T) -> Ok<T>;
template<class E> Error(E) -> Error<E>;

template<class T, class E>
struct Result {
    using OkT = Ok<T>;
    using ErrorE = Error<E>;

    Result() = delete;
    Result(const OkT& ok) : m(ok) {}
    Result(const ErrorE& error) : m(error) {}

    Result(OkT&& ok) : m(move(ok)) {}
    Result(ErrorE&& error) : m(move(error)) {}
        
    Result(const Result&) = default;
    Result& operator=(const Result&) = default;

    Result(Result&&) = default;
    Result& operator=(Result&&) = default;

    bool is_ok() const { return std::holds_alternative<Ok<T>>(m); }
    bool is_error() const { return std::holds_alternative<Error<E>>(m); }

    auto ok() const -> optional<std::reference_wrapper<const T>> {
        using R = optional<const T&>;
        return std::visit(overloaded {
            [](const Ok<T>& o) -> R { return std::cref(o.value); },
            [](const Error<E>& e) -> R { return {}; },
        }, m);
    }
    auto ok() -> optional<std::reference_wrapper<T>> {
        using R = optional<std::reference_wrapper<T>>;
        return std::visit(overloaded {
            [](Ok<T>& o) -> R { return std::ref(o.value); },
            [](Error<E>&) -> R { return {}; },
        }, m);
    }
    //TODO: auto err() const -> optional<std::reference_wrapper<const E>>;
    //TODO: auto err() -> optional<std::reference_wrapper<E>>;

    template<class F>
    [[nodiscard]]
    auto and_map(F&& f) {
        using R = return_type_t<F>;
        return std::visit(overloaded {
            [](Error<E>&& e) -> R { return e; },
            [ff = (F&&)f](Ok<T>&& o) -> R { return ff(o.value); },
        }, move(m));
    }

    //TODO: and
    //TODO: or
    //TODO: or_map

    auto unwrap() const -> const T& {
        return std::get<Ok<T>>(m).value;
    }

    auto unwrap_or(const T& t) const -> const T& {
        return std::visit(overloaded {
            [&](const Error<E>&) -> const T& { return t; },
            [](const Ok<T>& o) -> const T& { return o.value; },
        }, m);
    }

    template<class F>
    auto unwrap_or_map(F&& f) const -> const T& {
        return std::visit(overloaded {
            [ff = (F&&)f](const Error<E>& e) -> const T& { return ff(e.reason); },
            [](const Ok<T>& o) -> const T& { return o.value; },
        }, m);
    }

    // TODO: unwrap_err

private:
    std::variant<Ok<T>, Error<E>> m;
};
