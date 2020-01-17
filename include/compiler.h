#ifndef BELISH_COMPILER_H
#define BELISH_COMPILER_H

#include <map>
#include "ast.h"
using std::map;

#define MBDKV "\x01"
#define SBDKV "\x01"

namespace Belish {
    enum OPID {
        PUSH_NUM, PUSH_STR, PUSH_OBJ, PUSH_NULL, PUSH_UND, REFER, DEB, PUSH, ADD, SUB, MUL, DIV, MOD,
        EQ, NEQ, LEQ, MEQ, LESS, MORE,
        MAND, MOR, MXOR, MNOT,
        LAND, LOR, LNOT, POW
    };
    class Compiler {
    public:
        Compiler() { }
        Compiler(const string& s) : script(s), ast(s) { }
        void compile(string&);
        void compile_(string&);
    private:
        string script;
        AST ast;
        map<string, UL> sym;
        UL stkOffset = 0;
    };
}

#endif //BELISH_COMPILER_H
