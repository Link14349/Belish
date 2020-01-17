#include "trans.h"
#include "trans.cpp"
#include "compiler.h"
#include <ctime>

void Belish::Compiler::compile(string &bytecode) {
    bytecode = "\x9a\xd0\x75\x5c";// 魔数
    bytecode += MBDKV;
    bytecode += SBDKV;
    bytecode += transI64S_bin(time(nullptr));
    compile_(bytecode);
    bytecode += "\x06";
}

void Belish::Compiler::compile_(string &bytecode) {
    while (true) {
        if (!ast.child) ast.parse();
        if (!ast.root || ast.root->type() == Lexer::PROGRAM_END) break;
        switch (ast.root->type()) {
            case Lexer::NUMBER_TOKEN:
                bytecode += (char) OPID::PUSH_NUM;
                bytecode += transI64S_bin(transDI64_bin(transSD(ast.root->value())));
                break;
            case Lexer::STRING_TOKEN: {
                bytecode += (char) OPID::PUSH_STR;
                string val(ast.root->value());
                bytecode += transI32S_bin(val.length()) + val;
                break;
            }
            case Lexer::UNDEFINED_TOKEN: {
                bytecode += (char) OPID::PUSH_UND;
                break;
            }
            case Lexer::LET_TOKEN: {
                for (auto i = 0; i < ast.root->length(); i += 2) {
                    Compiler compiler;
                    compiler.ast.child = true;
                    compiler.ast.root = ast.root->get(i + 1);
                    compiler.compile_(bytecode);
                    sym[ast.root->get(i)->value()] = stkOffset++;
                }
                break;
            }
            default: {// 表达式
                Compiler compiler;
                compiler.ast.child = true;
                compiler.ast.root = ast.root->get(0);
                compiler.compile_(bytecode);
                compiler.ast.root = ast.root->get(1);
                compiler.compile_(bytecode);
                switch (ast.root->type()) {
                    case Lexer::ADD_TOKEN:
                        bytecode += (char)OPID::ADD;
                        break;
                    case Lexer::SUB_TOKEN:
                        bytecode += (char)OPID::SUB;
                        break;
                    case Lexer::MUL_TOKEN:
                        bytecode += (char) OPID::MUL;
                        break;
                    case Lexer::DIV_TOKEN:
                        bytecode += (char) OPID::DIV;
                        break;
                    case Lexer::MOD_TOKEN:
                        bytecode += (char) OPID::MOD;
                        break;
                    case Lexer::EQUAL_TOKEN:
                        bytecode += (char)OPID::EQ;
                        break;
                    case Lexer::NOT_EQUAL_TOKEN:
                        bytecode += (char)OPID::NEQ;
                        break;
                    case Lexer::LEQUAL_TOKEN:
                        bytecode += (char)OPID::LEQ;
                        break;
                    case Lexer::MEQUAL_TOKEN:
                        bytecode += (char)OPID::MEQ;
                        break;
                    case Lexer::LESS_TOKEN:
                        bytecode += (char)OPID::LESS;
                        break;
                    case Lexer::MORE_TOKEN:
                        bytecode += (char)OPID::MORE;
                        break;
                    case Lexer::MAND_TOKEN:
                        bytecode += (char)OPID::MAND;
                        break;
                    case Lexer::MOR_TOKEN:
                        bytecode += (char)OPID::MOR;
                        break;
                    case Lexer::MNOT_TOKEN:
                        bytecode += (char)OPID::MNOT;
                        break;
                    case Lexer::MXOR_TOKEN:
                        bytecode += (char)OPID::MXOR;
                        break;
                    case Lexer::LAND_TOKEN:
                        bytecode += (char)OPID::LAND;
                        break;
                    case Lexer::LOR_TOKEN:
                        bytecode += (char)OPID::LOR;
                        break;
                    case Lexer::LNOT_TOKEN:
                        bytecode += (char)OPID::LNOT;
                        break;
                    case Lexer::POWER_TOKEN:
                        bytecode += (char)OPID::POW;
                        break;
                }
                break;
            }
        }
        if (ast.child) break;
    }
}