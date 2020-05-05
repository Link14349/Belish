#include <iostream>
#include <Belish.h>
#include <cstdio>
using std::fstream;
using std::ios;

int main(int argc, char* argv[]) {
    remove("testmodule.belc");
    string script;
    Belish::readFile("test.bel", script);
    Belish::Compiler compiler("test", script);
    string bc;
    TIME_START(COMPILER)
    auto state = compiler.compile(bc, true);
    TIME_END(COMPILER)
    if (state) {
        std::cerr << "[Stop compiling]\n";
        return 1;
    }
    Belish::writeFile("test.belc", bc);
    std::cout << "finish compiling" << std::endl;
    ULL length;
    auto buffer = Belish::readFileCPTR("test.belc", length);
    Belish::BVM bvm("test", buffer, length);
    Belish::decompile(buffer, length);
    TIME_START(VM)
    bvm.run(Belish::Arg(argc, argv));
    TIME_END(VM)
    std::cout << "finish running" << std::endl;
    std::cout << "vm used " << GET_TIME(VM) << "ms, compiler used " << GET_TIME(COMPILER) << "ms" << std::endl;
    delete buffer;
//    buffer = Belish::readFileCPTR("testmodule.belc", length);
//    Belish::decompile(buffer, length);
//    delete buffer;
    return 0;
}