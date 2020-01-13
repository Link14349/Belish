#include <iostream>
#include "Belish.h"

int main(int argc, char* argv[]) {
    Belish::AST ast("a('123')");
    ast.parse();
    return 0;
}