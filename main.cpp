#include <iostream>
#include <Belish.h>
#include <fstream>
#include "timestamp.h"
#include <sstream>
using std::fstream;
using std::ios;

#define DEBUG 0

#if DEBUG

void* operator new(size_t s) {
    auto p = malloc(s);
    std::clog << "n" << p << std::endl;
    return p;
}
void operator delete(void* p) {
    std::clog << "d" << p << std::endl;
    free(p);
}
#endif

int main(int argc, char* argv[]) {
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
//    Belish::decompile(buffer, length);
    Belish::BVM bvm(buffer, length);
    bvm.run();
    std::cout << "finish running" << std::endl;
    auto e = getCurrentTime();
    std::cout << "vm used " << (e - s) << "ms, compiler used " << ct << "ms" << std::endl;
    delete buffer;
    return 0;
}