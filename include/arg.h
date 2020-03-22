#ifndef BELISH_ARG_H
#define BELISH_ARG_H

#include <iostream>
#include <set>
#include <map>
#include <list>
#include <string>

namespace Belish {
    class Arg {
    public:
        Arg(int, char*[]);
        void show() {
            for (auto& i : values) std::cout << i.first << ": " << i.second << std::endl;
            for (auto& i : flags) std::cout << i << ": true" << std::endl;
            for (auto& i : indValues) std::cout << i << std::endl;
        }
        size_t size() { return argc; }
        std::string* args() { return argv; }
        ~Arg() { delete[] argv; }
        std::map<std::string, std::string> values;
        std::set<std::string> flags;
        std::list<std::string> indValues;
    private:
        int argc;
        std::string* argv;
    };
}

#endif //BELISH_ARG_H
