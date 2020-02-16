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

char* readFileCPTR(const string& filename, ULL& length) {
    std::ifstream t;
    char* buffer;
    t.open("test.belc");
    t.seekg(0, std::ios::end);
    length = t.tellg();
    t.seekg(0, std::ios::beg);
    buffer = new char[length];
    t.read(buffer, length);
    t.close();
    return buffer;
}
void openFile(const string& filename, string& content) {
    fstream file(filename, ios::in | ios::out);
    if(!file)
    {
        std::cerr << "Failed.\n";
        file.close();
        exit(1);
    }
    std::stringstream buf;
    buf << file.rdbuf();
    content = buf.str();
    file.close();
}

int main(int argc, char* argv[]) {
    auto s = getCurrentTime();
    ULL ct;
    string script;
    openFile("test.bel", script);
    Belish::Compiler compiler("test.bel", script);
    std::ofstream fs;
    fs.open("test.belc", std::ios::out);
    string bc;
    auto state = compiler.compile(bc);
    if (state) {
        std::cerr << "[Stop compiling]\n";
        return 1;
    }
    fs << bc;
    fs.close();
    std::cout << "finish compiling" << std::endl;
    auto now = getCurrentTime();
    ct = now - s;
    s = now;
    ULL length;
    auto buffer = readFileCPTR("test.belc", length);
    Belish::decompile(buffer, length);
    Belish::BVM bvm(buffer, length);
    bvm.run();
    std::cout << "finish running" << std::endl;
    auto e = getCurrentTime();
    std::cout << "vm used " << (e - s) << "ms, compiler used " << ct << "ms" << std::endl;
    delete buffer;
    return 0;
}