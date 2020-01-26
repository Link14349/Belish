#include "trans.h"
#include "trans.cpp"
#include "compiler.h"
#include <ctime>
#include <iostream>

bool Belish::Compiler::compile(string &bytecode) {
    bytecode = "\x9a\xd0\x75\x5c";// 魔数
    bytecode += MBDKV;
    bytecode += SBDKV;
    bytecode += transI64S_bin(time(nullptr));
    auto state = compile_(bytecode);
    bytecode += "\x06";// 测试用，输出栈中情况
    return state;
}

bool Belish::Compiler::compile_(string &bytecode, bool inOPTOEXPR) {
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
            case Lexer::ERROR_TOKEN: {
                std::cerr << ast.root->value() << " at <" << filename << ">:" << ast.line() << std::endl;
                return true;
            }
            case Lexer::UNKNOWN_TOKEN: {
                auto oi = sym.find(ast.root->value());
//                std::cerr << (sym.find("a") == sym.end()) << ", " << (sym.find("b") == sym.end()) << std::endl;
                if (oi == sym.end()) {
                    std::cerr << "BLE100: Undefined symbol '" << ast.root->value() << "' at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
                auto offset = oi->second;
                if (inOPTOEXPR) bytecode += (char)OPID::REFER + transI32S_bin(offset);
                else bytecode += (char)OPID::PUSH + transI32S_bin(offset);
                break;
            }
            case Lexer::LET_TOKEN: {
                for (auto i = 0; i < ast.root->length(); i += 2) {
                    Compiler compiler(filename);
                    compiler.ast.child = true;
                    compiler.ast.root = ast.root->get(i + 1);
                    compiler.sym = sym;
                    if (compiler.compile_(bytecode)) {
                        return true;
                    }
                    sym.insert(std::pair<string, UL>(ast.root->get(i)->value(), stkOffset++));
                }
                break;
            }
            default: {// 表达式
                Compiler compiler(filename);
                compiler.ast.child = true;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                if (compiler.compile_(bytecode, ast.root->type() > Lexer::SRIGHT_TOKEN && ast.root->type() < Lexer::IN_TOKEN))
                    return true;
                compiler.ast.root = ast.root->get(1);
//                compiler.sym = sym;
                if (compiler.compile_(bytecode)) return true;
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
                    case Lexer::ADD_TO_TOKEN:
                        bytecode += (char)OPID::ADD;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::SUB_TO_TOKEN:
                        bytecode += (char)OPID::SUB;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::MUL_TO_TOKEN:
                        bytecode += (char) OPID::MUL;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::DIV_TO_TOKEN:
                        bytecode += (char) OPID::DIV;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::MOD_TO_TOKEN:
                        bytecode += (char) OPID::MOD;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::MAND_TO_TOKEN:
                        bytecode += (char)OPID::MAND;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::MOR_TO_TOKEN:
                        bytecode += (char)OPID::MOR;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::MXOR_TO_TOKEN:
                        bytecode += (char)OPID::MXOR;
//                        bytecode += (char)OPID::POP;
                        break;
                    case Lexer::POWER_TO_TOKEN:
                        bytecode += (char)OPID::POW;
//                        bytecode += (char)OPID::POP;
                        break;
                }
                if (!ast.child/* && ast.root->type() < Lexer::ADD_TO_TOKEN*/)
                    bytecode += (char)OPID::POP;
                break;
            }
        }
        if (ast.child) break;
    }
    return false;
}