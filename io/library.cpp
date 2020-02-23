#include "library.h"

#include <iostream>
#include <fstream>
#include <Belish.h>

Belish::Value* print(Belish::Stack* argv) {
    for (auto i = 0; i < argv->length(); i++) {
        if (argv->get(i)->type() == Belish::OBJECT) std::cout << argv->get(i)->toStringHL() << " ";
        else std::cout << argv->get(i)->toString() << " ";
    }
    return new Belish::Undefined;
}
Belish::Value* println(Belish::Stack* argv) {
    for (auto i = 0; i < argv->length(); i++) {
        if (argv->get(i)->type() == Belish::OBJECT) std::cout << argv->get(i)->toStringHL() << " ";
        else std::cout << argv->get(i)->toString() << " ";
    }
    std::cout << std::endl;
    return new Belish::Undefined;
}
Belish::Value* input(Belish::Stack* argv) {
    if (argv->length()) std::cout << argv->get(0)->toString();
    string str;
    std::cin >> str;
    return new Belish::String(str);
}
Belish::Value* write(Belish::Stack* argv) {
    if (argv->length() < 2) return new Belish::Boolean(false);
    string path = argv->get(0)->toString();
    string content = argv->get(1)->toString();
    std::ofstream fs(path, std::ios::trunc | std::ios::out);
    if (!fs.good()) {
        std::cout << path << std::endl;
        fs.close();
        return new Belish::Boolean(false);
    }
    fs << content;
    fs.close();
    return new Belish::Boolean(true);
}
Belish::Value* app(Belish::Stack* argv) {
    if (argv->length() < 2) return new Belish::Boolean(false);
    string path = argv->get(0)->toString();
    string content = argv->get(1)->toString();
    std::ofstream fs(path, std::ios::app | std::ios::out);
    if (!fs.good()) {
        fs.close();
        return new Belish::Boolean(false);
    }
    fs << content;
    fs.close();
    return new Belish::Boolean(true);
}

MODULE_SETUP_DEF {
    MODULE_SETUP_INIT(io)
    MODULE_SETUP_EXPORT("print", new Belish::NFunction(print))
    MODULE_SETUP_EXPORT("println", new Belish::NFunction(println))
    MODULE_SETUP_EXPORT("input", new Belish::NFunction(input))
    MODULE_SETUP_EXPORT("write", new Belish::NFunction(write))
    MODULE_SETUP_EXPORT("app", new Belish::NFunction(app))
    MODULE_SETUP_FINISH
}