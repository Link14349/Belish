#ifndef BELISH_VALUES_H
#define BELISH_VALUES_H

#include "trans.h"
#include <iostream>
#include <vector>
#include <cmath>
using std::vector;

namespace Belish {
    enum TYPE {
        NUMBER, STRING, OBJECT, NULL_, UNDEFINED
    };
    class Value {
    public:
        virtual TYPE type() = 0;
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
    private:
    };
    class Number : public Value {
    public:
        Number(double n = 0) : val(n) { }
        TYPE type() { return NUMBER; }
        string toString() { return std::to_string(val); }
        string toStringHL() { return "\033[33m" + std::to_string(val) + "\033[0m"; }
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
        double& value() { return val; }
    private:
        double val;
    };
    class String : public Value {
    public:
        String(const string& n = "") : val(n) { }
        TYPE type() { return STRING; }
        string toString() { return val; }
        string toStringHL() { return "\033[36m" + val + "\033[0m"; }
        string& value() { return val; }
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
    private:
        string val;
    };
    class Undefined : public Value {
    public:
        Undefined() { }
        TYPE type() { return UNDEFINED; }
        string toString() { return "undefined"; }
        string toStringHL() { return "\033[35mundefined\033[0m"; }
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
    private:
    };

    class Stack {
    public:
        Stack() : len(0) { val.resize(1024); }
        Value* get(ULL offset) { return val[offset < 0 ? 0 : offset]; }
        void set(ULL offset, Value* v) { val[offset] = v; }
        void push(Value* v);
        void pop(ULL offset) {
            len -= offset;
            if (val[len]->type() == NUMBER) delete val[len];
        }
        void dbg();
        ULL length() { return len; }
    private:
        vector<Value*> val;
        ULL len;
    };
}


#endif //BELISH_VALUES_H
