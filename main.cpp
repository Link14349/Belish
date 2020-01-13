#include <iostream>
#include "Belish.h"

int main(int argc, char* argv[]) {
    Belish::AST ast("b(123)(1 + 2, c(12, 8));");
    ast.parse();
    return 0;
}