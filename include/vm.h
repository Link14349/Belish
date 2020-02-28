#ifndef BELISH_VM_H
#define BELISH_VM_H

#include <string>
#include "values.h"
#include "compiler.h"
#include "dylib.h"
using std::string;

namespace Belish {
    class BVM {
    public:
        BVM(const string& fn, char* bc, ULL l) : filename(fn), bytecode(bc), len(l) { modules.reserve(16);frames.reserve(16); }
        void run();
        void Throw(UL errID, const string& message) {
            std::cerr << "BLE" << errID << ": " << message;
            auto iter = --callingLineStk.cend();
            std::cerr << " at <" << filename << ">:" << *(iter--) << "\n";
            while (iter != callingLineStk.cend()) std::cerr << " at <" << filename << ">:" << *(iter--) << "\n";
            error = true;
        }
        ~BVM() {
            delete stk;
        }
    private:
        bool error = false;
        bool child = false;
        bool callMoudleMethod = false;
        char* bytecode;
        UL functionLen;
        UL footerAdr;
        UL i;
        UL inFun = 0;
        ULL len;
        Stack* stk = nullptr;
        Value* regs[REG_COUNT] = { nullptr };
        vector<BVM*> modules;
        vector<Dylib*> exlibs;
        vector<Stack*> frames;
        vector<UL> functions;
        std::list<UL> callingLineStk;
        string filename;
    };
    typedef Object* (*ModuleSetup)();
}


#endif //BELISH_VM_H
