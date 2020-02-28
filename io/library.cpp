#include "library.h"

#include <iostream>
#include <fstream>
#include <sys/stat.h>
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
Belish::Value* writeFile(Belish::Stack* argv) {
    if (argv->length() < 2) return new Belish::Boolean(false);
    string path = argv->get(0)->toString();
    string content = argv->get(1)->toString();
    std::ofstream fs(path, std::ios::trunc | std::ios::out);
    if (!fs.good()) {
        fs.close();
        return new Belish::Boolean(false);
    }
    fs << content;
    fs.close();
    return new Belish::Boolean(true);
}
Belish::Value* appFile(Belish::Stack* argv) {
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
Belish::Value* readFile(Belish::Stack* argv) {
    if (argv->length() < 1) return new Belish::Boolean(false);
    string path = argv->get(0)->toString();
    string content;
    std::fstream file(path, std::ios::in | std::ios::out);
    if(!file)
    {
        file.close();
        return new Belish::Boolean(false);
    }
    std::stringstream buf;
    buf << file.rdbuf();
    content = buf.str();
    file.close();
    return new Belish::String(content);
}
Belish::Value* mkdir(Belish::Stack* argv) {
    if (argv->length() < 1) return new Belish::Boolean(false);
    string path = argv->get(0)->toString();
    mkdir(path.c_str(), 0755);
    return new Belish::Boolean(true);
}

MODULE_SETUP_DEF {
    MODULE_SETUP_INIT(io)
    MODULE_SETUP_EXPORT("print", new Belish::NFunction(print))
    MODULE_SETUP_EXPORT("println", new Belish::NFunction(println))
    MODULE_SETUP_EXPORT("input", new Belish::NFunction(input))
    MODULE_SETUP_EXPORT("writeFile", new Belish::NFunction(writeFile))
    MODULE_SETUP_EXPORT("appFile", new Belish::NFunction(appFile))
    MODULE_SETUP_EXPORT("readFile", new Belish::NFunction(readFile))
    MODULE_SETUP_EXPORT("mkdir", new Belish::NFunction(mkdir))
    MODULE_SETUP_FINISH
}