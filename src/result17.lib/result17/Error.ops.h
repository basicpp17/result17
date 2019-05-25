#pragma once
#include "Error.h"

namespace result17 {

template<class L, class R>
bool operator==(const Error<L>& l, const Error<R>& r) {
    return l.reason == r.reason;
}

template<class L, class R>
bool operator!=(const Error<L>& l, const Error<R>& r) {
    return l.reason != r.reason;
}

} // namespace result17
