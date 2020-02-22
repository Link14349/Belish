#include <iostream>
#include <Belish.h>
#include <fstream>
#include <cstdio>
#include "timestamp.h"
#include <sstream>
using std::fstream;
using std::ios;

int main(int argc, char* argv[]) {
    remove("testmodule.belc");
    auto s = getCurrentTime();
    ULL ct;
    string script;
    Belish::readFile("test.bel", script);
    Belish::Compiler compiler("test", script);
    string bc;
    auto state = compiler.compile(bc);
    if (state) {
        std::cerr << "[Stop compiling]\n";
        return 1;
    }
    Belish::writeFile("test.belc", bc);
    std::cout << "finish compiling" << std::endl;
    auto now = getCurrentTime();
    ct = now - s;
    s = now;
    ULL length;
    auto buffer = Belish::readFileCPTR("test.belc", length);
    Belish::BVM bvm(buffer, length);
//    Belish::decompile(buffer, length);
    bvm.run();
    std::cout << "finish running" << std::endl;
    auto e = getCurrentTime();
    std::cout << "vm used " << (e - s) << "ms, compiler used " << ct << "ms" << std::endl;
    delete buffer;
//    buffer = Belish::readFileCPTR("testmodule.belc", length);
//    Belish::decompile(buffer, length);
//    delete buffer;
    return 0;
}