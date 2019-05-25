#pragma once
#include "Ok.h"

namespace result17 {

template<class L, class R>
bool operator==(const Ok<L>& l, const Ok<R>& r) {
    return l.value == r.value;
}

template<class L, class R>
bool operator!=(const Ok<L>& l, const Ok<R>& r) {
    return l.value != r.value;
}

} // namespace result17
