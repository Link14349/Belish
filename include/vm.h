#ifndef BELISH_VM_H
#define BELISH_VM_H

#include <string>
using std::string;

namespace Belish {
    class BVM {
    public:
        BVM(char* bc, ULL l) : bytecode(bc), len(l) { }
        BVM(string& bc) : bytecode(bc.data()), len(bc.length()) { }
        void run();
    private:
        char* bytecode;
        ULL len;
    };
}


#endif //BELISH_VM_H
