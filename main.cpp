#include <iostream>
#include "Belish.h"

int main() {
    Belish::Lexer lexer("let I={age:18,name:\"Zelda\"};let me->I;let my=>I;print my age;");
    auto token = lexer.get();
    while (token.t != Belish::Lexer::PROGRAM_END) {
        std::cout << token.s << std::endl;
        token = lexer.get();
    }
    return 0;
}