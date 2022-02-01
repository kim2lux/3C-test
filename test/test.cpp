#include <tuple>
#include <iostream>

template<typename T, typename ... S>
void printTpl(T&& tpl, S&& ... args) {
    std::cout << std::get<0>(tpl) << std::endl;
    (std::cout << ... << args) << std::endl;
}

int main() {
    std::tuple tpl(10, 30.4, "hello");
    printTpl(tpl, 1, 2, 3);
}