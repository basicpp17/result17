template<class... Ts> 
struct overloaded : Ts... { 
    using Ts::operator()...;
};

// template deduction guide
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
