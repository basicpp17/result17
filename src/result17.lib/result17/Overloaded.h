#pragma once

/// simple tool to generate an overloaded lambda invokable
template<class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
Overloaded(Ts...)->Overloaded<Ts...>;
