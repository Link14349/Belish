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
    UL footerAdr_ = bytecode.length();
    bytecode += "0000";// 占位
    isRoot = true;
    auto state = compile_(bytecode);
    auto footerAdrS = transI32S_bin(footerAdr);
    bytecode[footerAdr_] = footerAdrS[0];
    bytecode[footerAdr_ + 1] = footerAdrS[1];
    bytecode[footerAdr_ + 2] = footerAdrS[2];
    bytecode[footerAdr_ + 3] = footerAdrS[3];
//    std::cout << "ast time: " << astTime << "ms" << std::endl;
    return state;
}

bool Belish::Compiler::compile_(string &bytecode, bool inOPTOEXPR, std::list<UL>* brTab, std::list<UL>* ctTab) {
//    ULL st = 0, ed = 0;
    newVars.clear();
    while (true) {
//        st = getCurrentTime();
        if (!ast.child) ast.parse();
//        ed = getCurrentTime();
//        astTime += ed - st;
        if (!ast.root || ast.root->type() == Lexer::PROGRAM_END) break;
        switch (ast.root->type()) {
            case Lexer::RETURN_TOKEN:
            {
                Compiler compiler(filename);
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.compile_(bytecode);
                bytecode += (char) BACK;
                break;
            }
            case Lexer::DEF_TOKEN:
            {
                functionAdrTab[ast.root->value()] = funOffset++;
                string funBytecode;
                Compiler compiler(filename);
                // 这里注释掉的内容是关于函数对定义处变量使用的实现的，这个再后再填回去
//                compiler.sym = sym;
//                compiler.stkOffset = stkOffset;
                compiler.ast.child = true;
                compiler.macro = macro;
                compiler.functionAdrTab = functionAdrTab;
                for (auto i = 0; i < ast.root->get(0)->length(); i++)
                    compiler.sym[ast.root->get(0)->get(i)->value()] = compiler.stkOffset++;
                for (auto i = 1; i < ast.root->length(); i++) {
                    compiler.ast.root = ast.root->get(i);
                    compiler.compile_(funBytecode);
                }
                funBytecode += (char) BACK;
                functionsBcs.push_back(funBytecode);
                break;
            }
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
            case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN: {
                Compiler compiler(filename);
                compiler.sym = sym;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.independent = false;
                compiler.ast.child = true;
                compiler.ast.root = ast.root->get(0);
                compiler.compile_(bytecode);
                compiler.ast.root = ast.root->get(1);
                compiler.compile_(bytecode);
                bytecode += (char) GET_ATTR;
                if (!inOPTOEXPR) {
                    bytecode += (char) SAV;
                    bytecode += (char) POP;
                    bytecode += (char) BAC;
                }
                break;
            }
            case Lexer::IF_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.stkOffset = stkOffset;
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
            case Lexer::NO_STATUS:
            case Lexer::END_TOKEN:
                break;
            case Lexer::FOR_TOKEN: {
                auto conAsts = ast.root->get(0);
                if (conAsts->length() != 3) {
                    std::cerr << "BLE202: Unexpected condition at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
                Compiler scCompiler(filename);
                scCompiler.ast.child = true;
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.stkOffset = stkOffset;
                scCompiler.ast.root = conAsts->get(0);
                scCompiler.compile_(bytecode);
                auto loopVars = scCompiler.newVars;
                for (auto & loopVar : loopVars) sym.insert(std::pair<string, UL>(loopVar, stkOffset++));
                auto conAdrS = transI32S_bin(bytecode.length());
                scCompiler.ast.root = conAsts->get(1);
                scCompiler.independent = false;
                scCompiler.compile_(bytecode);
                bytecode += (char) JF;
                auto conFAdr = bytecode.length();
                bytecode += "0000";// 占位
                scCompiler.independent = true;
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    scCompiler.ast.root = ast.root->get(i);
                    scCompiler.compile_(bytecode, false, &breakTab, &continueTab);
                }
//                bytecode += "\x06";
                auto lastConAdrS = transI32S_bin(bytecode.length());
                scCompiler.ast.root = conAsts->get(2);
                scCompiler.compile_(bytecode);
                bytecode += (char) JMP;
                bytecode += conAdrS;
                auto lastAdrS = transI32S_bin(bytecode.length());
                bytecode[conFAdr] = lastAdrS[0];
                bytecode[conFAdr + 1] = lastAdrS[1];
                bytecode[conFAdr + 2] = lastAdrS[2];
                bytecode[conFAdr + 3] = lastAdrS[3];
                for (auto i = breakTab.begin(); i != breakTab.end(); i++) {
                    bytecode[*i] = lastAdrS[0];
                    bytecode[*i + 1] = lastAdrS[1];
                    bytecode[*i + 2] = lastAdrS[2];
                    bytecode[*i + 3] = lastAdrS[3];
                }
                for (auto i = continueTab.begin(); i != continueTab.end(); i++) {
                    bytecode[*i] = lastConAdrS[0];
                    bytecode[*i + 1] = lastConAdrS[1];
                    bytecode[*i + 2] = lastConAdrS[2];
                    bytecode[*i + 3] = lastConAdrS[3];
                }
                bytecode += (char) POPC;
                bytecode += transI32S_bin(loopVars.size());
                stkOffset -= loopVars.size();
                for (auto & loopVar : loopVars) sym.erase(loopVar);
                break;
            }
            case Lexer::UNKNOWN_TOKEN: {
                auto oi = sym.find(ast.root->value());
                if (oi == sym.end()) {
                    auto om = macro.find(ast.root->value());
                    if (om == macro.end()) {
                        std::cerr << "BLE100: Undefined symbol '" << ast.root->value() << "' at <" << filename << ">:" << ast.line() << std::endl;
                        return true;
                    } else {
                        auto val = om->second;
                        Compiler compiler(filename, val);
                        compiler.sym = sym;
                        compiler.functionAdrTab = functionAdrTab;
                        compiler.macro = macro;
                        compiler.independent = false;
                        compiler.compile_(bytecode);
                        break;
                    }
                }
                auto offset = oi->second;
                if (inOPTOEXPR) bytecode += (char)OPID::REFER + transI32S_bin(offset);
                else bytecode += (char)OPID::PUSH + transI32S_bin(offset);
                if (independent) bytecode += (char) POP;
                break;
            }
            case Lexer::OBJECT_TOKEN:
            {
                bytecode += (char) PUSH_OBJ;
                for (UL i = 0; i < ast.root->length(); i += 2) {
                    string attr_name(ast.root->get(i)->value());
                    Compiler compiler(filename);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.sym = sym;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.ast.root = ast.root->get(i + 1);
                    compiler.compile_(bytecode);
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin(attr_name.length()) + attr_name;
                    bytecode += (char) SET_ATTR;
                }
                if (independent) bytecode += (char) POP;
                break;
            }
            case Lexer::DOT_TOKEN:
            {
                std::list<string> attrs;
                auto dotNode = ast.root;
                while (dotNode->type() == Lexer::DOT_TOKEN) {
                    attrs.push_front(dotNode->get(1)->value());
                    dotNode = dotNode->get(0);
                }
                Compiler compiler(filename);
                compiler.ast.root = dotNode;
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.sym = sym;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.compile_(bytecode);
                for (auto i = attrs.begin(); i != attrs.end(); i++) {
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin((*i).length()) + *i;
                    bytecode += (char) GET_ATTR;
                }
                if (!inOPTOEXPR) {
                    bytecode += (char) SAV;
                    bytecode += (char) POP;
                    bytecode += (char) BAC;
                }
                break;
            }
            case Lexer::LET_TOKEN: {
                for (UL i = 0; i < ast.root->length(); i += 2) {
                    if (ast.root->get(i + 1)->type() == Lexer::PN_IREFER_TOKEN) {
                        macro[ast.root->get(i)->value()] = ast.root->get(i + 1)->get(0)->value();
                        continue;
                    }
                    Compiler compiler(filename);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.ast.root = ast.root->get(i + 1)->get(0);
                    compiler.sym = sym;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.stkOffset = stkOffset;
                    if (compiler.compile_(bytecode, ast.root->get(i + 1)->type() == Lexer::PN_DREFER_TOKEN)) {
                        return true;
                    }
                    newVars.push_back(ast.root->get(i)->value());
                    sym.insert(std::pair<string, UL>(ast.root->get(i)->value(), stkOffset++));
                }
                break;
            }
            case Lexer::WHILE_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.stkOffset = stkOffset;
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
                auto conAdrS = transI32S_bin(conAdr);
                bytecode += (char) OPID::JMP;
                bytecode += conAdrS;
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
                for (auto i = continueTab.begin(); i != continueTab.end(); i++) {
                    bytecode[*i] = conAdrS[0];
                    bytecode[*i + 1] = conAdrS[1];
                    bytecode[*i + 2] = conAdrS[2];
                    bytecode[*i + 3] = conAdrS[3];
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
            case Lexer::CONTINUE_TOKEN:
            {
                if (ctTab) {
                    bytecode += (char) OPID::JMP;
                    ctTab->push_back(bytecode.length());
                    bytecode += "0000";// 占位
                } else {
                    std::cerr << "BLE201: Unexpected continue at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
            }
            case Lexer::DO_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.stkOffset = stkOffset;
                scCompiler.ast.child = true;
                UL bodyAdr = bytecode.length();
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    scCompiler.ast.root = ast.root->get(i);
                    scCompiler.compile_(bytecode, false, &breakTab, &continueTab);
                }
                scCompiler.independent = false;
                scCompiler.ast.root = ast.root->get(0);
                auto conAdrS = transI32S_bin(bytecode.length());
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
                for (auto i = continueTab.begin(); i != continueTab.end(); i++) {
                    bytecode[*i] = conAdrS[0];
                    bytecode[*i + 1] = conAdrS[1];
                    bytecode[*i + 2] = conAdrS[2];
                    bytecode[*i + 3] = conAdrS[3];
                }
                break;
            }
            default: {// 表达式
                Compiler compiler(filename);
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.stkOffset = stkOffset;
                if (ast.root->type() == Lexer::BRACKETS_LEFT_TOKEN) {
                    auto oi = functionAdrTab.find(ast.root->get(0)->value());
                    if (oi == functionAdrTab.end()) {
                        std::cerr << "BLE100: Undefined symbol '" << ast.root->get(0)->value() << "' at <" << filename << ">:" << ast.line() << std::endl;
                        return true;
                    }
                    for (UL i = 1; i < ast.root->length(); i++) {
                        compiler.ast.root = ast.root->get(i);
                        compiler.compile_(bytecode);
                    }
                    bytecode += (char) NEW_FRAME;
                    bytecode += transI32S_bin(ast.root->length() - 1);
                    bytecode += (char) CALL;
                    bytecode += transI32S_bin(oi->second);
                    break;
                }
                if (compiler.compile_(bytecode, ast.root->type() > Lexer::SRIGHT_TOKEN && ast.root->type() < Lexer::IN_TOKEN))
                    return true;
                if (ast.root->length() == 2) {
                    compiler.ast.root = ast.root->get(1);
                    if (compiler.compile_(bytecode)) return true;
                }
                switch (ast.root->type()) {
                    case Lexer::SLEFT_TOKEN:
                    case Lexer::SLEFT_TO_TOKEN:
                        bytecode += (char)OPID::SL;
                        break;
                    case Lexer::SRIGHT_TOKEN:
                    case Lexer::SRIGHT_TO_TOKEN:
                        bytecode += (char)OPID::SR;
                        break;
                    case Lexer::ADD_TOKEN:
                    case Lexer::ADD_TO_TOKEN:
                        bytecode += (char)OPID::ADD;
                        break;
                    case Lexer::SUB_TOKEN:
                    case Lexer::SUB_TO_TOKEN:
                        bytecode += (char)OPID::SUB;
                        break;
                    case Lexer::MUL_TOKEN:
                    case Lexer::MUL_TO_TOKEN:
                        bytecode += (char) OPID::MUL;
                        break;
                    case Lexer::DIV_TOKEN:
                    case Lexer::DIV_TO_TOKEN:
                        bytecode += (char) OPID::DIV;
                        break;
                    case Lexer::MOD_TOKEN:
                    case Lexer::MOD_TO_TOKEN:
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
                    case Lexer::MAND_TO_TOKEN:
                        bytecode += (char)OPID::MAND;
                        break;
                    case Lexer::MOR_TOKEN:
                    case Lexer::MOR_TO_TOKEN:
                        bytecode += (char)OPID::MOR;
                        break;
                    case Lexer::MNOT_TOKEN:
                        bytecode += (char)OPID::MNOT;
                        break;
                    case Lexer::MXOR_TOKEN:
                    case Lexer::MXOR_TO_TOKEN:
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
                    case Lexer::POWER_TO_TOKEN:
                    case Lexer::POWER_TOKEN:
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
    footerAdr = bytecode.length();
    if (!isRoot) return false;
    bytecode += transI32S_bin(functionsBcs.size());
    for (auto i = functionsBcs.begin(); i != functionsBcs.end(); i++) {
        bytecode += "0000";
    }
    for (UL i = 0; i < functionsBcs.size(); i++) {
        auto adrS = transI32S_bin(bytecode.length());
        bytecode[footerAdr + i * 4 + 4] = adrS[0];
        bytecode[footerAdr + i * 4 + 5] = adrS[1];
        bytecode[footerAdr + i * 4 + 6] = adrS[2];
        bytecode[footerAdr + i * 4 + 7] = adrS[3];
        bytecode += functionsBcs[i];
    }
    return false;
}