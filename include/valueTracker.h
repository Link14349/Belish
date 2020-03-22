#ifndef BELISH_VALUETRACKER_H
#define BELISH_VALUETRACKER_H

#include <map>
#include "values.h"
#include "ast.h"
using std::map;

namespace Belish {
    class ValueTracker {
    public:
        ValueTracker() { }
        void set(const string& name, Value* val) {
            values[name] = val;
            val->bind();
        }
        void set(const string& name, double n) {
            if (has(name)) get(name)->linked--;
            auto val = values[name] = new Number(n);
            val->bind();
        }
        bool has(const string& name) { return values.find(name) != values.end(); }
        void track(AST::node*, bool = false);
        Value* get(const string& name) { return values[name]; }
    private:
        map<string, Value*> values;
    };
}

#endif //BELISH_VALUETRACKER_H
