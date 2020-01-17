#include <iostream>
#include <Belish.h>
#include <fstream>
#include <sys/time.h>
int64_t getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#define VM 0

int main(int argc, char* argv[]) {
    auto s = getCurrentTime();
#if VM
    std::ifstream t;
    ULL length;
    char* buffer;
    t.open("test.belc");
    t.seekg(0, std::ios::end);
    length = t.tellg();
    t.seekg(0, std::ios::beg);
    buffer = new char[length];
    t.read(buffer, length);
    t.close();
    Belish::BVM bvm(buffer, length);
    bvm.run();
#else
    Belish::Compiler compiler("let a = (1 + 15) ** (2 * (1 + 3) / 8) / 2, b = 'Haha -- \\'ZYH\\'' * 10;");
    std::ofstream fs;
    fs.open("test.belc", std::ios::out);
    string bc;
    compiler.compile(bc);
    fs << bc;
    fs.close();
#endif
    auto e = getCurrentTime();
    std::cout << "used " << (e - s) << "ms" << std::endl;
    return 0;
}