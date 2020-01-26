#include <iostream>
#include <Belish.h>
#include <fstream>
#include <sys/time.h>
#include <cstdlib>
int64_t getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#define DEBUG 0

#if DEBUG

UL c = 0;

void* operator new(size_t s) {
    c++;
    auto p = malloc(s);
    std::clog << "n" << p << std::endl;
    return p;
}
void operator delete(void* p) {
    c++;
    std::clog << "d" << p << std::endl;
    free(p);
}
#endif

int main(int argc, char* argv[]) {
    auto s = getCurrentTime();
//    Belish::Compiler compiler("test.bel", "let a \n= (1 + 15) ** \n(2 * (1 + 3) / 8\n) / 2, \nb = 'Haha -- \\'ZYH\\'' * 10 ; a += 10;b += '123';a + 1000;");
    Belish::Compiler compiler("test.bel", "let a \n= 8, \nb = 'Haha -- \\'ZYH\\'' * 10 ; a += 10;b += '123';a + 1000;");
    std::ofstream fs;
    fs.open("test.belc", std::ios::out);
    string bc;
    auto state = compiler.compile(bc);
    if (state) {
        std::cerr << "[Stop compiling, error-code: " << state << "]\n";
        return 1;
    }
    fs << bc;
    fs.close();
    std::cout << "finish compiling" << std::endl;
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
    std::cout << "finish running" << std::endl;
    auto e = getCurrentTime();
    std::cout << "used " << (e - s) << "ms" << std::endl;
    delete buffer;
    return 0;
}