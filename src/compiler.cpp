#include "trans.h"
#include "trans.cpp"
#include "compiler.h"
#include "timestamp.h"
#include <ctime>
#include <list>
#include <iostream>

//UL astTime = 0;

bool Belish::Compiler::compile(string &bytecode) {
    bytecode = "\x9a\xd0\x75\x5c";// 魔数
    bytecode += MBDKV;
    bytecode += SBDKV;
    bytecode += transI64S_bin(time(nullptr));
    auto state = compile_(bytecode);
    bytecode += "\x06";// 测试用，输出栈中情况
//    std::cout << "ast time: " << astTime << "ms" << std::endl;
    return state;
}

bool Belish::Compiler::compile_(string &bytecode, bool inOPTOEXPR, std::list<UL>* brTab, std::list<UL>* ctTab) {
//    ULL st = 0, ed = 0;
    while (true) {
//        st = getCurrentTime();
        if (!ast.child) ast.parse();
//        ed = getCurrentTime();
//        astTime += ed - st;
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
            case Lexer::IF_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.ast.child = true;
                std::list<UL> tags;
                const auto ASTL = ast.root->length();
                bool isLast(false);
                for (UL i = 0; i < ASTL; i += 2) {
                    if (ast.root->get(i)->type() == Lexer::NUMBER_TOKEN) {
                        // 恒真条件的判断在前面是因为恒真条件出现的可能性远大于恒假条件，可以有轻微的加速
                        if (ast.root->get(i)->value() != "0") isLast = true;// 说明该elif条件必然为true，接下来的elif/else是不可能执行了
                        else continue;// 条件恒为false的elif是不可能执行的
                    } else if (ast.root->get(i)->type() == Lexer::STRING_TOKEN) {
                        // 恒真条件的判断在前面是因为恒真条件出现的可能性远大于恒假条件，可以有轻微的加速
                        if (!ast.root->get(i)->value().empty()) isLast = true;// 说明该elif条件必然为true，接下来的elif/else是不可能执行了
                        else continue;// 条件恒为false的elif是不可能执行的
                    }
                    UL adr;
                    if (!isLast) {
                        scCompiler.independent = false;
                        scCompiler.ast.root = ast.root->get(i);
                        scCompiler.compile_(bytecode);
                        bytecode += (char) OPID::JF;
                        adr = bytecode.length();
                        bytecode += "0000";// 先占位
                    }
                    scCompiler.independent = true;
                    for (UL j = 0; j < ast.root->get(i + 1)->length(); j++) {
                        scCompiler.ast.root = ast.root->get(i + 1)->get(j);
                        scCompiler.compile_(bytecode, false, brTab, ctTab);
                    }
                    if (isLast) break;
                    // 跳到最后
                    bytecode += (char) OPID::JMP;
                    tags.push_back(bytecode.length());
                    bytecode += "0000";// 先占位
                    // 将之前占的位的正确的值填回去
                    auto targetStr = transI32S_bin(bytecode.length());
                    bytecode[adr] = targetStr[0];
                    bytecode[adr + 1] = targetStr[1];
                    bytecode[adr + 2] = targetStr[2];
                    bytecode[adr + 3] = targetStr[3];
                }
                auto targetAdr = transI32S_bin(bytecode.length());
                for (auto i = tags.begin(); i != tags.end(); i++) {
                    bytecode[*i] = targetAdr[0];
                    bytecode[*i + 1] = targetAdr[1];
                    bytecode[*i + 2] = targetAdr[2];
                    bytecode[*i + 3] = targetAdr[3];
                }
                break;
            }
            case Lexer::ERROR_TOKEN: {
                std::cerr << ast.root->value() << " at <" << filename << ">:" << ast.line() << std::endl;
                return true;
            }
            case Lexer::END_TOKEN: {
                break;
            }
            case Lexer::NO_STATUS: {
                break;
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
                for (UL i = 0; i < ast.root->length(); i += 2) {
                    Compiler compiler(filename);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.ast.root = ast.root->get(i + 1);
                    compiler.sym = sym;
                    if (compiler.compile_(bytecode)) {
                        return true;
                    }
                    sym.insert(std::pair<string, UL>(ast.root->get(i)->value(), stkOffset++));
                }
                break;
            }
            case Lexer::WHILE_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.ast.child = true;
                scCompiler.independent = false;
                scCompiler.ast.root = ast.root->get(0);
                UL conAdr = bytecode.length();
                scCompiler.compile_(bytecode);
                bytecode += (char) OPID::JF;
                UL JLastAdr = bytecode.length();
                bytecode += "0000";// 先占位
                scCompiler.independent = true;
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    scCompiler.ast.root = ast.root->get(i);
                    scCompiler.compile_(bytecode, false, &breakTab, &continueTab);
                }
                bytecode += (char) OPID::JMP;
                bytecode += transI32S_bin(conAdr);
                auto lastAdrS = transI32S_bin(bytecode.length());
                bytecode[JLastAdr] = lastAdrS[0];
                bytecode[JLastAdr + 1] = lastAdrS[1];
                bytecode[JLastAdr + 2] = lastAdrS[2];
                bytecode[JLastAdr + 3] = lastAdrS[3];
                for (auto i = breakTab.begin(); i != breakTab.end(); i++) {
                    bytecode[*i] = lastAdrS[0];
                    bytecode[*i + 1] = lastAdrS[1];
                    bytecode[*i + 2] = lastAdrS[2];
                    bytecode[*i + 3] = lastAdrS[3];
                }
                break;
            }
            case Lexer::BREAK_TOKEN:
            {
                if (brTab) {
                    bytecode += (char) OPID::JMP;
                    brTab->push_back(bytecode.length());
                    bytecode += "0000";// 占位
                } else {
                    std::cerr << "BLE200: Unexpected break at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
            }
            case Lexer::DO_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.ast.child = true;
                UL bodyAdr = bytecode.length();
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    scCompiler.ast.root = ast.root->get(i);
                    scCompiler.compile_(bytecode, false, brTab, ctTab);
                }
                scCompiler.independent = false;
                scCompiler.ast.root = ast.root->get(0);
                scCompiler.compile_(bytecode);
                bytecode += (char) OPID::JT;
                bytecode += transI32S_bin(bodyAdr);
                auto lastAdrS = transI32S_bin(bytecode.length());
                for (auto i = breakTab.begin(); i != breakTab.end(); i++) {
                    bytecode[*i] = lastAdrS[0];
                    bytecode[*i + 1] = lastAdrS[1];
                    bytecode[*i + 2] = lastAdrS[2];
                    bytecode[*i + 3] = lastAdrS[3];
                }
                break;
            }
            default: {// 表达式
                Compiler compiler(filename);
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                if (compiler.compile_(bytecode, ast.root->type() > Lexer::SRIGHT_TOKEN && ast.root->type() < Lexer::IN_TOKEN))
                    return true;
                if (ast.root->length() == 2) {
                    compiler.ast.root = ast.root->get(1);
                    if (compiler.compile_(bytecode)) return true;
                }
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
                        break;
                    case Lexer::SUB_TO_TOKEN:
                        bytecode += (char)OPID::SUB;
                        break;
                    case Lexer::MUL_TO_TOKEN:
                        bytecode += (char) OPID::MUL;
                        break;
                    case Lexer::DIV_TO_TOKEN:
                        bytecode += (char) OPID::DIV;
                        break;
                    case Lexer::MOD_TO_TOKEN:
                        bytecode += (char) OPID::MOD;
                        break;
                    case Lexer::MAND_TO_TOKEN:
                        bytecode += (char)OPID::MAND;
                        break;
                    case Lexer::MOR_TO_TOKEN:
                        bytecode += (char)OPID::MOR;
                        break;
                    case Lexer::MXOR_TO_TOKEN:
                        bytecode += (char)OPID::MXOR;
                        break;
                    case Lexer::POWER_TO_TOKEN:
                        bytecode += (char)OPID::POW;
                        break;
                    case Lexer::SET_TOKEN:
                        bytecode += (char)OPID::MOV;
                        break;
                    case Lexer::DADD_TOKEN:
                    case Lexer::DSUB_TOKEN:
                        bytecode += (char)OPID::SAV;
                        bytecode += (char)OPID::PUSH_NUM;
                        bytecode += transI64S_bin(transDI64_bin(1));
                        if (ast.root->type() == Lexer::DADD_TOKEN) bytecode += (char)OPID::ADD;
                        else bytecode += (char)OPID::SUB;
                        bytecode += (char)OPID::POP;
                        bytecode += (char)OPID::BAC;
                        break;
                }
                if (independent)
                    bytecode += (char)OPID::POP;
                break;
            }
        }
        if (ast.child) break;
    }
    return false;
}