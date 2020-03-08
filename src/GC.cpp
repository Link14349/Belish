#include <vm.h>
#include <set>
#include <stack>
#include "values.h"
#include "GC.h"

void Belish::GC::gc() {
    auto& deathObjects = bvm->deathObjects;
    auto& objects = bvm->objects;
    std::set<void*> objectSet;
    for (auto & object : objects) {
        objectSet.insert(object.first);
        std::stack<Object::Iterator> stk;
        stk.push(((Object*)object.first)->begin());
        while (!stk.empty()) {
            if (stk.top().end()) {
                stk.pop();
                while (!stk.empty() && stk.top().end()) stk.pop();
                if (stk.empty()) break;
                stk.top().next();
                continue;
            }
            if (stk.top().value()->type() == OBJECT) {
                if (objectSet.find(stk.top().value()) != objectSet.end()) {
                    stk.top().next();
                    continue;
                }
                objectSet.insert(stk.top().value());
                stk.push(((Object*)stk.top().value())->begin());
            } else stk.top().next();
        }
    }
    std::set<void*> objMap;
    for (auto& object : deathObjects) {
        if (objectSet.find(object) == objectSet.end() && objMap.find(object) == objMap.end()) {// 该object不可达且未被清理
            objMap.insert(object);
            object->del(objMap);
            free(object);
        }
    }
    deathObjects.clear();
}
