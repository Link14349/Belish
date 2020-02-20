#ifndef BELISH_VM_H
#define BELISH_VM_H

#include <string>
#include "values.h"
using std::string;

namespace Belish {
    class BVM {
    public:
        BVM(char* bc, ULL l) : bytecode(bc), len(l) { modules.reserve(16);frames.reserve(16); }
        void run();
        ~BVM() {
            for (auto i = modules.begin(); i != modules.end(); i++) {
                delete (*i)->stk;
                delete (*i)->bytecode;
            }
        }
    private:
        bool child = false;
        bool callMoudleMethod = false;
        char* bytecode;
        UL functionLen;
        UL footerAdr;
        UL i;
        UL inFun = 0;
        ULL len;
        Stack* stk = nullptr;
        vector<BVM*> modules;
        vector<Stack*> frames;
        vector<UL> functions;
    };
}


#endif //BELISH_VM_H
