#pragma once
#include "Ok.h"

#include <ostream>

namespace result17 {

template<class T>
auto operator<<(std::ostream& out, const Ok<T>& o) -> std::ostream& {
    return out << "Ok: " << o.value;
}

} // namespace result17
