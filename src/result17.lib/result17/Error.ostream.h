#pragma once
#include "Error.h"

#include <ostream>

namespace result17 {

template<class E>
auto operator<<(std::ostream& out, const Error<E>& e) -> std::ostream& {
    return out << "Error: " << e.reason;
}

} // namespace result17
