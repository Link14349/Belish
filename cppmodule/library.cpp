#include "library.h"

#include <iostream>
#include <Belish.h>

Belish::Value* print(Belish::Stack* argv) {
    for (auto i = 0; i < argv->length(); i++) {
        std::cout << argv->get(i)->toString() << " ";
    }
    return new Belish::Undefined;
}
Belish::Value* println(Belish::Stack* argv) {
    for (auto i = 0; i < argv->length(); i++) {
        std::cout << argv->get(i)->toString() << " ";
    }
    std::cout << std::endl;
    return new Belish::Undefined;
}

MODULE_SETUP_DEF {
    MODULE_SETUP_INIT(cppmodule)
    MODULE_SETUP_EXPORT("print", new Belish::NFunction(print))
    MODULE_SETUP_EXPORT("println", new Belish::NFunction(println))
    MODULE_SETUP_FINISH
}