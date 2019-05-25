#pragma once
#include "Result.h"

#include "Error.ostream.h"
#include "Ok.ostream.h"

namespace result17 {

template<class T, class E>
auto operator<<(std::ostream& out, const Result<T, E>& r) -> std::ostream& {
    return std::visit(
        Overloaded{
            [&](const Ok<T>& o) -> decltype(auto) { return out << "Result(" << o << ')'; }, //
            [&](const Error<E>& e) -> decltype(auto) { return out << "Result(" << e << ')'; } //
        },
        r.m);
}

} // namespace result17
