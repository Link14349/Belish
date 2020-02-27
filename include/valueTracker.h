#ifndef BELISH_VALUETRACKER_H
#define BELISH_VALUETRACKER_H

#include <map>
#include "values.h"
#include "ast.h"
using std::map;

namespace Belish {
    TYPE_VAL_ENUM_DEF(VTRACKER, UNKNOWN, _T)
    class TrackerValue {
    public:
        TrackerValue() : linked(0) { }
        void bind() { ++linked; }
        void unbind() {
            --linked;
            if (!linked) delete this;
        }
    private:
        VTRACKER type;
        UL linked;
    };
    class TrackerNumber : public TrackerValue {
    public:
        TrackerNumber() : TrackerValue() { }
        TrackerNumber(double n) : TrackerValue(), val(n) { }
        double& value() { return val; }
    private:
        double val;
    };

    class ValueTracker {
    public:
        ValueTracker() { }
        void set(const string& name, TrackerValue* val) {
            values[name] = val;
            val->bind();
        }
        void set(const string& name, double n) {
            if (has(name)) get(name)->unbind();
            auto val = values[name] = new TrackerNumber(n);
            val->bind();
        }
        bool has(const string& name) { return values.find(name) != values.end(); }
        void track(AST::node*);
        TrackerValue* get(const string& name) { return values[name]; }
    private:
        map<string, TrackerValue*> values;
    };
}

#endif //BELISH_VALUETRACKER_H
