#pragma once
#include "Result.h"

#include "Error.ops.h"
#include "Ok.ops.h"

namespace result17 {

template<class LV, class LE, class RV, class RE>
bool operator==(const Result<LV, LE>& l, const Result<RV, RE>& r) {
    return l.m == r.m;
}
template<class LV, class LE, class RV, class RE>
bool operator!=(const Result<LV, LE>& l, const Result<RV, RE>& r) {
    return l.m == r.m;
}

template<class LV, class LE, class RV>
bool operator==(const Result<LV, LE>& l, const Ok<RV>& r) {
    return l.andMap([r](const LV& v) { return v == r.value; }).unwrapOr(false);
}
template<class LV, class LE, class RV>
bool operator!=(const Result<LV, LE>& l, const Ok<RV>& r) {
    return !(l == r);
}

template<class LV, class LE, class RE>
bool operator==(const Result<LV, LE>& l, const Error<RE>& r) {
    return l
        .andMap([](auto&&) { return false; }) //
        .orMap([r](const LE& e) { return e == r.reason; })
        .unwrapOr(false);
}
template<class LV, class LE, class RE>
bool operator!=(const Result<LV, LE>& l, const Error<RE>& r) {
    return !(l == r);
}

} // namespace result17
