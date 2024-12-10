#include <iostream>

#include "tester.hpp"

int main() {
    Tester tester("input.txt");
    std::cout << tester.RunTest();
}