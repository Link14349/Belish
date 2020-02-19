#ifndef BELISH_VALUES_H
#define BELISH_VALUES_H

#include "trans.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
using std::vector;

namespace Belish {
    enum TYPE {
        NUMBER, STRING, OBJECT, UNDEFINED, FUNCTION, INT
    };
    // ***该类的引用计数只有Stack类有权操作它***
    class Value {
    public:
        virtual TYPE type() = 0;
        virtual Value* copy() = 0;
        virtual string toString() = 0;
        virtual string toStringHL() = 0;// 高亮、缩进显示
        virtual void add(Value*) = 0;
        virtual void sub(Value*) = 0;
        virtual void mul(Value*) = 0;
        virtual void div(Value*) = 0;
        virtual void mod(Value*) = 0;
        virtual void eq(Value*) = 0;
        virtual void neq(Value*) = 0;
        virtual void leq(Value*) = 0;
        virtual void meq(Value*) = 0;
        virtual void less(Value*) = 0;
        virtual void more(Value*) = 0;
        virtual void mand(Value*) = 0;
        virtual void mor(Value*) = 0;
        virtual void mxor(Value*) = 0;
        virtual void land(Value*) = 0;
        virtual void lor(Value*) = 0;
        virtual void pow(Value*) = 0;
        virtual void shiftl(Value*) = 0;
        virtual void shiftr(Value*) = 0;
        virtual bool isTrue() = 0;
        virtual bool isFalse() = 0;
        virtual void set(Value*) = 0;
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
        void shiftl(Value* n) override { val = (((ULL)val) << ((ULL)((Number*)n)->value())); }
        void shiftr(Value* n) override { val = (((ULL)val) >> ((ULL)((Number*)n)->value())); }
        bool isTrue() override { return val != 0; }
        bool isFalse() override { return val == 0; }
        uint64_t& value() { return val; }
    private:
        uint64_t val;
    };
    class String : public Value {
    public:
        String(const string& n = "") : val(n) { linked = 0; }
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
    class Undefined : public Value {
    public:
        Undefined() { linked = 0; }
        TYPE type() { return UNDEFINED; }
        string toString() { return "undefined"; }
        string toStringHL() { return "\033[35mundefined\033[0m"; }
        Value* copy() override { return new Undefined; }
        void add(Value* n) override { ; }
        void sub(Value* n) override { ; }
        void mul(Value* n) override { ; }
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
        void set(Value* n) override { ; }
        void shiftl(Value* n) override { ; }
        void shiftr(Value* n) override { ; }
        bool isTrue() override { return false; }
        bool isFalse() override { return true; }
    private:
    };
#define CLASS_TYPE_PROP_NAME "__type__"
    class Object : public Value {
    public:
        Object() { linked = 0; }
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
        void add(Value* n) override { ; }
        void sub(Value* n) override { ; }
        void mul(Value* n) override { ; }
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
        void shiftl(Value* n) override { ; }
        void shiftr(Value* n) override { ; }
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
        ~Object() {
            for (auto & i : prop) {
                i.second->linked--;
                if (i.second->linked == 0) delete i.second;
            }
        }
        class Iterator {
        public:
            Iterator(const Iterator& iterator) : binding(iterator.binding), iter(iterator.iter) { }
            bool next() {
                if (iter == binding->prop.end()) return false;
                iter++;
                return iter == binding->prop.end();
            }
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
        Function(UL i = 0, UL m = 0) : index(i), module(m) { linked = 0; }// module默认为0, 0代表自己
        UL id() { return index; }
        TYPE type() { return FUNCTION; }
        string toString() { return "<function #" + std::to_string(index + module) + ">"; }
        string toStringHL() { return "\033[35m<function #" + std::to_string(index + module) + ">\033[0m"; }
        Value* copy() override { return new Function(index, module); }
        void add(Value* n) override { ; }
        void sub(Value* n) override { ; }
        void mul(Value* n) override { ; }
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
        void set(Value* n) override { ; }
        void shiftl(Value* n) override { ; }
        void shiftr(Value* n) override { ; }
        bool isTrue() override { return true; }
        bool isFalse() override { return false; }
    private:
        UL index;
        UL module;
        friend class BVM;
    };

    class Stack {
    public:
        Stack() : len(0) { val.reserve(1024); }
        Value* get(UL offset) { if (offset < len) return val[offset < 0 ? 0 : offset]; else return nullptr; }
        void set(UL offset, Value* v) {
            if (!(--val[offset]->linked)) delete val[offset];
            val[offset] = v;
            v->linked++;
        }
        void push(Value* v);
        void pop(UL offset) {
            len -= offset;
            for (UL i = 0; i < offset; i++) {
                if (!(--val[len + i]->linked)) delete val[len + i];
                val[len + i] = nullptr;
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
        vector<Value*> val;
        UL len;
    };
}


#endif //BELISH_VALUES_H
