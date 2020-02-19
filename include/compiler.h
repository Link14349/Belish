#ifndef BELISH_COMPILER_H
#define BELISH_COMPILER_H

#include <map>
#include <list>
#include "ast.h"
using std::map;

#define MBDKV "\x01"
#define SBDKV "\x01"

namespace Belish {
    enum OPID {
        PUSH_NUM = 0, PUSH_STR, PUSH_OBJ, PUSH_NULL, PUSH_UND, REFER, DEB, PUSH, ADD, SUB, MUL, DIV, MOD,
        EQ, NEQ, LEQ, MEQ, LESS, MORE,
        MAND, MOR, MXOR, MNOT,
        LAND, LOR, LNOT, POW,
        MOV, POP, POPC, JT, JF, JMP,
        SAV, BAC, SL, SR, SET_ATTR, GET_ATTR, NEW_FRAME, CALL, RET, RESIZE, CHANGE, PUSH_FUN, CALL_FUN, IMP
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
        bool parentIsSet = false;
        UL pushedFunID;
        UL argCount = 0;
        string funName;
        UL funStart;
        map<string, UL> sym;
        map<string, string> macro;
        map<string, UL> functionAdrTab;
        std::list<string> newVars;
        bool independent = true;
    };
}

#endif //BELISH_COMPILER_H
