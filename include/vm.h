#ifndef BELISH_VM_H
#define BELISH_VM_H

#include <string>
#include <map>
#include <set>
#include <MAsmJit.h>
#include "values.h"
#include "compiler.h"
#include "dylib.h"
#include "GC.h"
#include "arg.h"
using std::string;

namespace Belish {
    class BVM {
        const UL ASMJIT_MACHINE_CODE_LEN = 65536;
    public:
        BVM(const string& fn, char* bc, ULL l) : filename(fn), bytecode(bc), len(l), gc(this), asmJit(ASMJIT_MACHINE_CODE_LEN) { modules.reserve(16);frames.reserve(16); }
        void run(const Arg&);
        void jitCompile(size_t, size_t);
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
        bool change_target_bcadr_flag = false;
        bool error = false;
        bool child = false;
        bool callModuleMethod = false;
        UL functionLen;
        UL footerAdr;
        UL i;
        UL inFun = 0;
        UL THRESHOLD = 1000;// 作为热点代码被编译的阈值为1000次被跳到
        ULL len;
        char* bytecode;
        GC gc;
        map<string, bool>* importedTab = nullptr;
        Stack* stk = nullptr;
        Value* regs[REG_COUNT] = { nullptr };
        MAsmJit::MAsmJit asmJit;
        std::map<void*, UL> objects;
        std::set<Object*> deathObjects;
        vector<BVM*> modules;
        vector<Dylib*> exlibs;
        vector<Stack*> frames;
        vector<UL> functions;
        std::set<Value*> ints;
        std::list<UL> callingLineStk;
        string filename;
        friend class GC;
    };
    typedef Object* (*ModuleSetup)();
}


#endif //BELISH_VM_H
