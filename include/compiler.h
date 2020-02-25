#ifndef BELISH_COMPILER_H
#define BELISH_COMPILER_H

#include <map>
#include <list>
#include "ast.h"
#include "values.h"
using std::map;

#define MBDKV "\x01"
#define SBDKV "\x01"

namespace Belish {
#define REG_COUNT 32
    enum OPID {
        PUSH_NUM = 0, PUSH_STR, PUSH_OBJ, PUSH_NULL, PUSH_UND, REFER, DEB, PUSH, ADD, SUB, MUL, DIV, MOD,
        EQ, NEQ, LEQ, MEQ, LESS, MORE,
        MAND, MOR, MXOR, MNOT,
        LAND, LOR, LNOT, POW,
        MOV, POP, POPC, JT, JF, JMP,
        SAV, BAC, SL, SR, SET_ATTR, GET_ATTR, NEW_FRAME, CALL, RET, RESIZE, CHANGE, PUSH_FUN, CALL_FUN, IMP, LOAD,
        REG_EQ, REG_NEQ, REG_LEQ, REG_MEQ, REG_LESS, REG_MORE, MOV_REG, REG_ADD, REG_SUB, PUSH_TRUE, PUSH_FALSE
    };
    class Compiler {
    public:
        Compiler() : filename("untitled") { }
        Compiler(const string& fn) : filename(fn) { }
        Compiler(const string& fn, const string& s) : script(s), ast(s), filename(fn) { }
        bool compile(string&);
        bool compile_(string&, bool = false, std::list<UL>* = nullptr, std::list<UL>* = nullptr);
    private:

        UL stkOffset = 0;
        UL funOffset = 0;
        string filename;
        string script;
        AST ast;
        UL footerAdr;
        bool isRoot = false;
        bool pushedFun = false;
        bool compilingForLoopCon = false;
        bool compilingForLoopUpd = false;
        bool parentIsSet = false;
//        bool leftValue = false;
        UL pushedFunID;
        UL argCount = 0;
        string funName;
        UL funStart;
        char regCount = REG_COUNT - 1;
        map<string, char> regVar;
        map<string, VariableValue*>* valueTracking;
        map<string, string> links;
        map<string, char> regValue;
        map<string, UL> sym;
        map<string, string> macro;
        map<string, UL> functionAdrTab;
        std::list<string> newVars;
        bool independent = true;
    };
}

#endif //BELISH_COMPILER_H
