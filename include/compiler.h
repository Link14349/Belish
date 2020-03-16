#ifndef BELISH_COMPILER_H
#define BELISH_COMPILER_H

#include <map>
#include <list>
#include "ast.h"
#include "values.h"
#include "valueTracker.h"
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
        SAV, BAC, SL, SR, SET_ATTR, GET_ATTR, NEW_FRAME_AND_CALL, CALL, RET, RESIZE, CHANGE, PUSH_FUN, CALL_FUN, IMP, LOAD,
        REG_EQ, REG_NEQ, REG_LEQ, REG_MEQ, REG_LESS, REG_MORE, MOV_REG, REG_ADD, REG_SUB, PUSH_TRUE, PUSH_FALSE,
        PUSH_OUTER, DEF_FUN, DEF_FUN_AND_PUSH,
        GET_CP_ATTR, NEW_FRAME_AND_CALL_AND_CALL_FUN, GET_ATTR_STR, GET_CP_ATTR_STR,
        LOAD_SUPER_METHOD, CREATE_ARRAY,

        LINE = 0xff
    };
#define CLASS_LINK ".class_link"
    class Compiler {
    public:
        Compiler() : filename("untitled") { }
        Compiler(const string& fn) : filename(fn) { }
        Compiler(const string& fn, const string& s) : script(s), ast(s), filename(fn) { }
        bool compile(string&, bool = false);// releaseType: 设置是否编译出代码行数等信息，true不编译，false编译
        bool compile_(string&, bool = false, bool = false, std::list<UL>* = nullptr, std::list<UL>* = nullptr);
        bool has(const string& name) { return sym.find(name) != sym.end(); }
        UL get(const string& name) { return sym[name]; }
    private:
        bool isEntry = true;
        bool isRoot = false;
        bool isBlock = false;
        bool pushedFun = false;
        bool compilingForLoopCon = false;
        bool compilingForLoopUpd = false;
        bool parentIsSet = false;
        bool independent = true;
        char regCount = REG_COUNT - 1;
        UL stkOffset = 0;
        UL funOffset = 0;
        UL footerAdr;
        UL pushedFunID;
        UL argCount = 0;
        UL nowLine = 0;
        UL funStart;
        ValueTracker* tracker;
        Compiler* parent = nullptr;
        string funName;
        string filename;
        string script;
        map<string, char> regVar;
        map<string, char> regValue;
        map<string, bool>* compiled;
        map<string, UL> sym;
        map<string, string> macro;
        map<string, UL> functionAdrTab;
        map<string, bool> needOut;// 所需要的函数定义处的值
        std::list<string> newVars;
        std::list<UL> outerUsingList;
        AST ast;
    };
#define SUPER_CLASS_ATTR_NAME ".super"
}

#endif //BELISH_COMPILER_H
