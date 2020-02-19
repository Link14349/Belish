#ifndef BELISH_VM_H
#define BELISH_VM_H

#include <string>
#include "values.h"
using std::string;

namespace Belish {
    class BVM {
    public:
        BVM(char* bc, ULL l) : bytecode(bc), len(l) { modules.reserve(16); }
        BVM(string& bc) : bytecode(bc.data()), len(bc.length()) { modules.reserve(16); }
        void run();
        ~BVM() {
            for (auto i = modules.begin(); i != modules.end(); i++) {
                delete (*i)->stk;
                delete (*i)->bytecode;
            }
        }
    private:
        bool child = false;
        char* bytecode;
        UL footerAdr;
        UL i;
        ULL len;
        Stack* stk = nullptr;
        vector<BVM*> modules;
        vector<UL> functions;
    };
}


#endif //BELISH_VM_H
