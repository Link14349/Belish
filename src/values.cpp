#include "values.h"

void Belish::Stack::push(Belish::Value *v) {
    if (len < val.size()) val[len++] = v;
    else {
        val.push_back(v);
        len++;
    }
    v->linked++;
}

void Belish::Stack::dbg() {
    std::cout << "[STACK(" << len << ")]" << std::endl;
    for (ULL i = 0; i < len; i++)
        std::cout << "$" << i << "(" << val[i] << ")\t" << val[i]->toStringHL() << std::endl;
    std::cout << "===========" << std::endl;
}
