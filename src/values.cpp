#include "values.h"

void Belish::Stack::push(Belish::Value *v) {
    if (len < val.size()) val[len++] = v;
    else {
        val.push_back(v);
        len++;
    }
}

void Belish::Stack::dbg() {
    for (ULL i = 0; i < len; i++) {
        std::cout << "$" << i << "\t" << val[i]->toStringHL() << std::endl;
    }
}
