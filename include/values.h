#ifndef BELISH_VALUES_H
#define BELISH_VALUES_H

#include "trans.h"
#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <map>
#include <list>
#include <set>

using std::vector;

namespace Belish {
#define TYPE_VAL_ENUM_DEF(TYPE, ADDTION, BAC) \
    enum TYPE { \
        NUMBER##BAC, STRING##BAC, OBJECT##BAC, UNDEFINED##BAC, FUNCTION##BAC, INT##BAC, NFUNCTION##BAC, BOOLEAN##BAC, ADDTION##BAC \
    };
    TYPE_VAL_ENUM_DEF(TYPE, , )
    class Value {
    public:
        Value() { /*std::clog << "n" << this << "\n";*/ }
        virtual ~Value() { /*std::clog << "d" << this << "\n";*/ }
        virtual TYPE type() = 0;
        virtual Value* copy() = 0;
        virtual string toString() = 0;
        virtual string toStringHL() = 0;// 高亮、缩进显示
        virtual void add(Value*) { ; }
        virtual void sub(Value*) { ; }
        virtual void mul(Value*) { ; }
        virtual void div(Value* ) { ; }
        virtual void mod(Value* ) { ; }
        virtual void eq(Value* ) { ; }
        virtual void neq(Value* ) { ; }
        virtual void leq(Value* ) { ; }
        virtual void meq(Value* ) { ; }
        virtual void less(Value* ) { ; }
        virtual void more(Value* ) { ; }
        virtual void mand(Value* ) { ; }
        virtual void mor(Value* ) { ; }
        virtual void mxor(Value* ) { ; }
        virtual void land(Value* ) { ; }
        virtual void lor(Value* ) { ; }
        virtual void pow(Value* ) { ; }
        virtual void shiftl(Value* ) { ; }
        virtual void shiftr(Value* ) { ; }
        virtual void lnot() { ; }
        virtual void mnot() { ; }
        virtual bool isTrue( ) { return true; }
        virtual bool isFalse( ) { return false; }
        virtual void set(Value* ) { ; }
        UL count() { return linked; }
    protected:
        friend class Stack;
        friend class BVM;
        friend class Object;
        UL linked;
    };
    class Number : public Value {
    public:
        Number(double n = 0) : val(n) { linked = 0;  }
        virtual ~Number() {}
        TYPE type() { return NUMBER; }
        string toString() { return std::to_string(val); }
        string toStringHL() { return "\033[33m" + std::to_string(val) + "\033[0m"; }
        Value* copy() override { return new Number(val); }
        void add(Value* n) override { val += ((Number*)n)->val; }
        void sub(Value* n) override { val -= ((Number*)n)->val; }
        void mul(Value* n) override { val *= ((Number*)n)->val; }
        void div(Value* n) override { val /= ((Number*)n)->val; }
        void mod(Value* n) override { val = ((LL)val) % ((LL)((Number*)n)->val); }
        void eq(Value* n) override { val = val == ((Number*)n)->val; }
        void neq(Value* n) override { val = val != ((Number*)n)->val; }
        void leq(Value* n) override { val = val <= ((Number*)n)->val; }
        void meq(Value* n) override { val = val >= ((Number*)n)->val; }
        void less(Value* n) override { val = val < ((Number*)n)->val; }
        void more(Value* n) override { val = val > ((Number*)n)->val; }
        void mand(Value* n) override { val = ((LL)val) & ((LL)((Number*)n)->val); }
        void mor(Value* n) override { val = ((LL)val) | ((LL)((Number*)n)->val); }
        void mxor(Value* n) override { val = ((LL)val) ^ ((LL)((Number*)n)->val); }
        void land(Value* n) override { val = val && ((Number*)n)->val; }
        void lor(Value* n) override { val = val || ((Number*)n)->val; }
        void pow(Value* n) override { val = std::pow(val, ((Number*)n)->val); }
        void set(Value* n) override { val = ((Number*)n)->val; }
        void shiftl(Value* n) override { val = (((ULL)val) << ((ULL)((Number*)n)->value())); }
        void shiftr(Value* n) override { val = (((ULL)val) >> ((ULL)((Number*)n)->value())); }
        bool isTrue() override { return val != 0; }
        bool isFalse() override { return val == 0; }
        double& value() { return val; }
    private:
        double val;
    };
    class Int : public Value {
    public:
        Int(double n = 0) : val(n) { linked = 0;  }
        virtual ~Int() {}
        TYPE type() { return INT; }
        string toString() { return std::to_string(val); }
        string toStringHL() { return "\033[33m" + std::to_string(val) + "\033[0m"; }
        Value* copy() override { return new Number(val); }
        void add(Value* n) override { val += ((Int*)n)->val; }
        void sub(Value* n) override { val -= ((Int*)n)->val; }
        void mul(Value* n) override { val *= ((Int*)n)->val; }
        void div(Value* n) override { val /= ((Int*)n)->val; }
        void mod(Value* n) override { val %= ((Int*)n)->val; }
        void eq(Value* n) override { val = val == ((Int*)n)->val; }
        void neq(Value* n) override { val = val != ((Int*)n)->val; }
        void leq(Value* n) override { val = val <= ((Int*)n)->val; }
        void meq(Value* n) override { val = val >= ((Int*)n)->val; }
        void less(Value* n) override { val = val < ((Int*)n)->val; }
        void more(Value* n) override { val = val > ((Int*)n)->val; }
        void mand(Value* n) override { val &= ((Int*)n)->val; }
        void mor(Value* n) override { val |= ((Int*)n)->val; }
        void mxor(Value* n) override { val ^= ((Int*)n)->val; }
        void land(Value* n) override { val = val && ((Int*)n)->val; }
        void lor(Value* n) override { val = val || ((Int*)n)->val; }
        void pow(Value* n) override { val = std::pow(val, ((Int*)n)->val); }
        void set(Value* n) override { val = ((Int*)n)->val; }
        void shiftl(Value* n) override { val = (val << (((Int*)n)->value())); }
        void shiftr(Value* n) override { val = (val >> (((Int*)n)->value())); }
        virtual void lnot() { val = !val; }
        virtual void mnot() { val = ~val; }
        bool isTrue() override { return val != 0; }
        bool isFalse() override { return val == 0; }
        uint64_t& value() { return val; }
    private:
        uint64_t val;
    };
    class String : public Value {
    public:
        String(const string& n = "") : val(n) { linked = 0; }
        virtual ~String() {}
        TYPE type() { return STRING; }
        string toString() { return val; }
        string toStringHL() { return "\033[36m\"" + val + "\"\033[0m"; }
        string& value() { return val; }
        Value* copy() override { return new String(val); }
        void add(Value* n) override { val += ((String*)n)->val; }
        void sub(Value* n) override { ; }
        void mul(Value* n) override {
            UL count = ((Number*)n)->value() - 1;
            string tmp(val);
            for (auto i = 0; i < count; i++) {
                val += tmp;
            }
        }
        void div(Value* n) override { ; }
        void mod(Value* n) override { ; }
        void eq(Value* n) override { ; }
        void neq(Value* n) override { ; }
        void leq(Value* n) override { ; }
        void meq(Value* n) override { ; }
        void less(Value* n) override { ; }
        void more(Value* n) override { ; }
        void mand(Value* n) override { ; }
        void mor(Value* n) override { ; }
        void mxor(Value* n) override { ; }
        void land(Value* n) override { ; }
        void lor(Value* n) override { ; }
        void pow(Value* n) override { ; }
        void set(Value* n) override { val = ((String*)n)->val; }
        void shiftl(Value* n) override {
            ULL count = ((Number*)n)->value();
            val.erase(0, count);
        }
        void shiftr(Value* n) override {
            ULL count = ((Number*)n)->value();
            val.erase(val.length() - count, count);
        }
        bool isTrue() override { return !val.empty(); }
        bool isFalse() override { return val.empty(); }
    private:
        string val;
    };
    class Boolean : public Value {
    public:
        Boolean(bool n = true) : val(n) { linked = 0; }
        virtual ~Boolean() {}
        TYPE type() { return STRING; }
        string toString() { return val ? "true" : "false"; }
        string toStringHL() { return "\033[36m" + string(val ? "true" : "false") + "\033[0m"; }
        bool& value() { return val; }
        Value* copy() override { return new Boolean(val); }
        void eq(Value* n) override { val = val == ((Boolean*)n)->val; }
        void neq(Value* n) override { val = val != ((Boolean*)n)->val; }
        void mand(Value* n) override { val = val & ((Boolean*)n)->val; }
        void mor(Value* n) override { val = val | ((Boolean*)n)->val; }
        void mxor(Value* n) override { val = val ^ ((Boolean*)n)->val; }
        void land(Value* n) override { val = val && ((Boolean*)n)->val; }
        void lor(Value* n) override { val = val || ((Boolean*)n)->val; }
        void set(Value* n) override { val = ((Boolean*)n)->val; }
        virtual void lnot() { val = !val; }
        virtual void mnot() { val = ~val; }
        bool isTrue() override { return val; }
        bool isFalse() override { return !val; }
    private:
        bool val;
    };
    class Undefined : public Value {
    public:
        Undefined() { linked = 0; }
        virtual ~Undefined() {}
        TYPE type() { return UNDEFINED; }
        string toString() { return "undefined"; }
        string toStringHL() { return "\033[35mundefined\033[0m"; }
        Value* copy() override { return new Undefined; }
        bool isTrue() override { return false; }
        bool isFalse() override { return true; }
    private:
    };
    class Object : public Value {
    private:
        void del(std::set<void*>& objSet) {
            std::clog << "d" << this << "\n";
            for (auto& i : prop) {
                i.second->linked--;
                if (i.second->linked == 0) {
                    if (i.second->type() == OBJECT) {
                        if (objSet.find(i.second) != objSet.end()) continue;
                        objSet.insert(i.second);
                        ((Object*)i.second)->del(objSet);
                        free(i.second);
                        continue;
                    }
                    delete i.second;
                }
            }
        }
        friend class GC;
    public:
        Object() { linked = 0; std::clog << "n" << this << "\n"; }
        ~Object() override {// 只有普通的没有循环引用的Object才会直接调析构函数
            std::clog << "d" << this << "\n";
            for (auto& i : prop) {
                i.second->linked--;
                if (i.second->linked == 0) delete i.second;
            }
        }
        TYPE type() { return OBJECT; }
        string toString() {
            return "\n" + toString("");
        }
        string toString(const string& tab) {
            string res("{");
            for (auto i = prop.begin(); i != prop.end(); i++) {
                res += "\n" + tab + "\t'" + i->first + "': ";
                if (i->second->type() == OBJECT) res += ((Object*)i->second)->toString(tab + "\t");
                else res += i->second->toString();
            }
            res += "\n" + tab + "}";
            return res;
        }
        string toStringHL(const string& tab) {
            string res(tab + "{");
            for (auto i = prop.begin(); i != prop.end(); i++) {
                res += "\n" + tab + "\t\033[35m'" + i->first + "'\033[0m: ";
                if (i->second->type() == OBJECT) res += ((Object*)i->second)->toStringHL(tab + "\t");
                else res += i->second->toStringHL();
            }
            res += "\n" + tab + "}";
            return res;
        }
        string toStringHL() {
            return "\n" + toStringHL("");
        }
        Value* copy() override {
            auto obj = new Object;
            for (auto i = prop.begin(); i != prop.end(); i++) {
                obj->set(i->first, i->second);
            }
            return obj;
        }
        void set(Value* n) override {
            auto obj = (Object*)n;
            for (auto & i : prop) {
                i.second->linked--;
                if (i.second->linked == 0) delete i.second;
            }
            for (auto & i : obj->prop) {
                i.second->linked++;
                prop[i.first] = i.second;
            }
        }
        bool isTrue() override { return !prop.empty(); }
        bool isFalse() override { return prop.empty(); }
        Value* operator[](const string& k) {
            return prop[k];
        }
        Value* get(const string& k) {
            return prop[k];
        }
        void set(const string& k, Value* val) {
            auto i = prop.find(k);
            if (i != prop.end() && i->second) {
                i->second->linked--;
                if (i->second->linked == 0) delete i->second;
            }
            prop[k] = val;
            val->linked++;
        }
        class Iterator {
        public:
            Iterator(const Iterator& iterator) : binding(iterator.binding), iter(iterator.iter) { }
            bool next() {
                if (iter == binding->prop.end()) return true;
                iter++;
                return iter == binding->prop.end();
            }
            bool end() { return iter == binding->prop.end(); }
            string key() { return iter->first; }
            Value* value() { return iter->second; }
        private:
            explicit Iterator(Object* b) : binding(b), iter(b->prop.begin()) { }
            explicit Iterator(Object* b, std::map<string, Value*>::iterator iter) : binding(b), iter(iter) { }
            Object* binding;
            std::map<string, Value*>::iterator iter;
            friend class Object;
        };
        Iterator begin() { return Iterator(this); }
        Iterator end() { return Iterator(this, prop.end()); }
    private:
        std::map<string, Value*> prop;
    };
    class Function : public Value {
    public:
        Function(UL i = 0, UL m = 0, bool ism = false) : index(i), module(m), isMethod(ism) { linked = 0; }// module默认为0, 0代表自己
        virtual ~Function() {}
        UL id() { return index; }
        TYPE type() { return FUNCTION; }
        string toString() { return "<function #" + std::to_string(index + module) + ">"; }
        string toStringHL() { return "\033[35m<function #" + std::to_string(index + module) + ">\033[0m"; }
        Value* copy() override { return new Function(index, module, isMethod); }
        bool isTrue() override { return true; }
        bool isFalse() override { return false; }
    private:
        UL index;
        UL module;
        bool isMethod;
        friend class BVM;
    };

    class Stack;
    typedef Value* (*_NFunction)(Stack*);
    class NFunction : public Value {// Native Function
    public:
        NFunction(_NFunction nFunction = nullptr) : fun(nFunction) { linked = 0; }// module默认为0, 0代表自己
        virtual ~NFunction() {}
        TYPE type() { return NFUNCTION; }
        string toString() { return "<NativeFunction>"; }
        string toStringHL() { return "\033[35m<NativeFunction>\033[0m"; }
        Value* copy() override { return new NFunction(fun); }
        Value* call(Stack* stack) { return fun(stack); }
        bool good() { return fun; }
        bool isTrue() override { return true; }
        bool isFalse() override { return false; }
    private:
        _NFunction fun;
    };
    class NValue : public Value {// Native Value
    public:
        NValue(void* v = nullptr) : val(v) { linked = 0; }// module默认为0, 0代表自己
        virtual ~NValue() {}
        TYPE type() { return NFUNCTION; }
        string toString() { return "<NativeValue>"; }
        string toStringHL() { return "\033[35m<NativeValue>\033[0m"; }
        Value* copy() override { return new NValue(val); }
        bool isTrue() override { return true; }
        bool isFalse() override { return false; }
    private:
        void* val;
    };

    class Stack {
    public:
        Stack(std::map<void*, UL>& objs, std::list<Object*>& dobjs) : len(0), objects(objs), deathObjects(dobjs) { val.reserve(1024); }
        Value* get(UL offset) { if (offset < len) return val[offset < 0 ? 0 : offset]; else return nullptr; }
        void set(UL offset, Value* v) {
            auto& value = val[offset];
            if (!(--val[offset]->linked)) {
                delete value;
                if (value->type() == OBJECT) objects.erase(value);
            } else if (value->type() == OBJECT && !(--objects[value])) {
                deathObjects.push_back((Object*)value);
                objects.erase(value);
            }
            value = v;
            v->linked++;
        }
        void push(Value* v);
        void pop(UL offset) {
            len -= offset;
            for (UL i = 0; i < offset; i++) {
                auto& value = val[len + i];
                if (!(--value->linked)) {
                    delete value;
                    if (value->type() == OBJECT) objects.erase(value);
                } else if (value->type() == OBJECT && !(--objects[value])) {
                    deathObjects.push_back((Object*)value);
                    objects.erase(value);
                }
                value = nullptr;
            }
        }
        void dbg();
        Value* top() {
            return val[len - 1];
        }
        UL length() { return len; }
        ~Stack() {
            for (UL i = 0; i < len; i++)
                if (!(--val[i]->linked)) delete val[i];
        }
    private:
        UL len;
        std::map<void*, UL>& objects;
        std::list<Object*>& deathObjects;
        vector<Value*> val;
        friend class NFunction;
    };
}


#endif //BELISH_VALUES_H