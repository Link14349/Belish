#ifndef BELISH_GC_H
#define BELISH_GC_H

#include "vm.h"

namespace Belish {
    class BVM;
    class GC {
    public:
        GC(BVM* vm) : bvm(vm) { }
        void gc();
    private:
        BVM* bvm;
    };
}


#endif //BELISH_GC_H
