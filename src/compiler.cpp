#include "trans.h"
#include "trans.cpp"
#include "compiler.h"
#include "timestamp.h"
#include <ctime>
#include <list>
#include "fio.h"
#include <iostream>

//UL astTime = 0;

bool Belish::Compiler::compile(string &bytecode, bool releaseType) {
    if (isEntry) compiled = new map<string, bool>;
    bytecode = "\x9a\xd0\x75\x5c";// 魔数
    bytecode += MBDKV;
    bytecode += SBDKV;
    bytecode += transI64S_bin(time(nullptr));
    UL footerAdr_ = bytecode.length();
    bytecode += "0000";// 占位
    isRoot = true;
    isBlock = true;
    tracker = new ValueTracker;
    // ===============
    // 添加__exports__变量
    tracker->set("__exports__", new Unknown);
    tracker->set("process", new Unknown);
    sym["__exports__"] = stkOffset++;
    bytecode += (char) INIT_MODULE_INFO;
    bytecode += transI32S_bin(filename.length());
    bytecode += filename;
    // 添加process变量
    sym["process"] = stkOffset++;
//    bytecode += (char) PUSH_OBJ;
//    //设置其的属性
//    bytecode += (char) PUSH_STR;
//    bytecode += transI32S_bin(8);
//    bytecode += "filename";
//    bytecode += (char) PUSH_STR;
//    bytecode += transI32S_bin(filename.length());
//    bytecode += filename;
//    bytecode += (char) SET_ATTR;
    // ===============
    newVars.clear();
    auto state = compile_(bytecode, releaseType, false, nullptr, nullptr);
    delete tracker;
    auto footerAdrS = transI32S_bin(footerAdr);
    bytecode[footerAdr_] = footerAdrS[0];
    bytecode[footerAdr_ + 1] = footerAdrS[1];
    bytecode[footerAdr_ + 2] = footerAdrS[2];
    bytecode[footerAdr_ + 3] = footerAdrS[3];
//    std::cout << "ast time: " << astTime << "ms" << std::endl;
    if (isEntry) delete compiled;
    return state;
}

bool Belish::Compiler::compile_(string &bytecode, bool releaseType, bool inOPTOEXPR, std::list<UL>* brTab, std::list<UL>* ctTab) {
//    ULL st = 0, ed = 0;
#define COM_COM_SET_NOW_LINE(compiler) compiler.nowLine = nowLine;
    std::list<AST::node*> functionAsts;
    std::list<map<string, UL> > functionDefSym;
    if (isRoot) {
        if ((*compiled)[filename]) return false;
        (*compiled)[filename] = true;
    }
    char sign = 0;// 传信变量
    /*
     * 1. 用于判断是否在编译new实例化对象
     *      - 1: 在编译new实例化对象 0: 不在编译new实例化对象
     *
     * **: 所有表示否的都是0
     * */
    while (true) {
//        st = getCurrentTime();
        if (!ast.child) ast.parse();
//        ed = getCurrentTime();
//        astTime += ed - st;
        if (!ast.root || ast.root->type() == Lexer::PROGRAM_END) break;
        if (tracker) tracker->track(ast.root);
        if (!releaseType && ast.line() > nowLine) {
            nowLine = ast.line();
            bytecode += (char) LINE;
            bytecode += transI32S_bin(nowLine);
        }
        COM_A_COM_ORG:
        switch (ast.root->type()) {
            case Lexer::ARRAY_TOKEN:
            {
                for (auto i = 0; i < ast.root->length(); i++) {
                    Compiler compiler(filename);
                    COM_COM_SET_NOW_LINE(compiler);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.ast.root = ast.root->get(i);
                    compiler.sym = sym;
                    compiler.regCount = regCount;
                    compiler.regValue = regValue;
                    compiler.regVar = regVar;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.compile_(bytecode, releaseType);
                }
                bytecode += (char) CREATE_ARRAY;
                bytecode += transI32S_bin(ast.root->length());
                break;
            }
            case Lexer::NEW_TOKEN:
            {
                auto ctorNodeDot = new AST::node(Lexer::DOT_TOKEN, "", ast.root->line());
                ctorNodeDot->insert(ast.root->get(0));
                ctorNodeDot->insert(Lexer::UNKNOWN_TOKEN, "ctor", ast.root->line());
                ast.root->noDelSet(0, ctorNodeDot);
                ast.root->type(Lexer::BRACKETS_LEFT_TOKEN);
                goto COM_A_COM_ORG;
            }
            case Lexer::CLASS_TOKEN:
            {
                string className(ast.root->value());
                bytecode += (char) PUSH_OBJ;
                bytecode += (char) PUSH_STR;
                bytecode += transI32S_bin(5);
                bytecode += ".name";
                bytecode += (char) PUSH_STR;
                bytecode += transI32S_bin(className.length());
                bytecode += className;
                bytecode += (char) SET_ATTR;
                for (UL i = 1; i < ast.root->length(); i++) {
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin(ast.root->get(i)->value().length());
                    bytecode += ast.root->get(i)->value();
                    bytecode += (char) DEF_FUN_AND_PUSH;
                    bytecode += transI32S_bin(funOffset);
                    if (ast.root->get(i)->value() == "ctor") {
//                        ast.root->get(i)->insert(Lexer::GLOBAL_TOKEN, "", ast.root->get(i)->get(-1)->line());
//                        ast.root->get(i)->get(-1)->insert(Lexer::UNKNOWN_TOKEN, className, ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->insert(Lexer::SET_TOKEN, "", ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->get(-1)->insert(Lexer::DOT_TOKEN, "", ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->get(-1)->get(0)->insert(Lexer::UNKNOWN_TOKEN, "this", ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->get(-1)->get(0)->insert(Lexer::UNKNOWN_TOKEN, CLASS_LINK, ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->get(-1)->insert(Lexer::UNKNOWN_TOKEN, className, ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->insert(Lexer::RETURN_TOKEN, "", ast.root->get(i)->get(-1)->line());
                        ast.root->get(i)->get(-1)->insert(Lexer::UNKNOWN_TOKEN, "this", ast.root->get(i)->get(-1)->line());
                    }
                    functionAdrTab["." + className + "." + ast.root->get(i)->value()] = funOffset++;
                    functionAsts.push_back(ast.root->get(i));
                    functionDefSym.push_back(sym);
                    bytecode += (char) SET_ATTR;
                    ast.root->noDelSet(i, nullptr);
                }
                sym[className] = stkOffset++;
                newVars.push_back(className);
                bytecode += (char) PUSH_STR;
                bytecode += transI32S_bin(string(SUPER_CLASS_ATTR_NAME).length()) + SUPER_CLASS_ATTR_NAME;
                if (ast.root->get(0)->type() == Lexer::NO_STATUS) {
                    bytecode += (char) PUSH_UND;
                    bytecode += (char) SET_ATTR;
                } else {
                    Compiler compiler(filename);
                    COM_COM_SET_NOW_LINE(compiler);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.ast.root = ast.root->get(0);
                    compiler.sym = sym;
                    compiler.regCount = regCount;
                    compiler.regValue = regValue;
                    compiler.regVar = regVar;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.compile_(bytecode, releaseType);
                    bytecode += (char) SET_ATTR;
                    bytecode += (char) LOAD_SUPER_METHOD;
                }
                break;
            }
            case Lexer::RETURN_TOKEN:
            {
                Compiler compiler(filename);
                COM_COM_SET_NOW_LINE(compiler);
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                compiler.regCount = regCount;
                compiler.regValue = regValue;
                compiler.regVar = regVar;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                if (ast.root->get(0)->type() == Lexer::BRACKETS_LEFT_TOKEN && funName == ast.root->get(0)->get(0)->value()) {
                    if (ast.root->get(0)->length() - 1 < argCount) {
                        for (UL i = 0; i < argCount - ast.root->get(0)->length() + 1; i++) {
                            ast.root->get(0)->insert(Lexer::UNDEFINED_TOKEN, "", ast.line());
                        }
                    }
                    for (UL i = 1; i < ast.root->get(0)->length() && i <= argCount; i++) {
                        compiler.ast.root = ast.root->get(0)->get(i);
                        if (compiler.compile_(bytecode, releaseType, true)) return true;
                    }
                    for (UL i = 1; i < ast.root->get(0)->length() && i <= argCount; i++) {
                        bytecode += (char) CHANGE;
                        bytecode += transI32S_bin(argCount - i);
                    }
                    bytecode += (char) JMP;
                    bytecode += transI32S_bin(funStart);
                } else {
                    if (compiler.compile_(bytecode, releaseType)) return true;
                    bytecode += (char) RET;
                }
                break;
            }
            case Lexer::DEF_TOKEN:
            {
                bytecode += (char) DEF_FUN;
                bytecode += transI32S_bin(funOffset);
                functionAdrTab[ast.root->value()] = funOffset++;
                functionAsts.push_back(ast.root);
                functionDefSym.push_back(sym);
                ast.root = nullptr;
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
                COM_COM_SET_NOW_LINE(compiler);
                compiler.sym = sym;
                compiler.regCount = regCount;
                compiler.regValue = regValue;
                compiler.regVar = regVar;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.independent = false;
                compiler.ast.child = true;
                compiler.ast.root = ast.root->get(0);
                if (compiler.compile_(bytecode, releaseType)) return true;
                compiler.ast.root = ast.root->get(1);
                if (compiler.compile_(bytecode, releaseType)) return true;
                if (!parentIsSet) {
                    if (inOPTOEXPR) bytecode += (char) GET_ATTR;
                    else bytecode += (char) GET_CP_ATTR;
                }
                break;
            }
            case Lexer::IF_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.regCount = regCount;
                scCompiler.regValue = regValue;
                scCompiler.regVar = regVar;
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
                        scCompiler.isBlock = false;
                        scCompiler.independent = false;
                        scCompiler.ast.root = ast.root->get(i);
                        if (scCompiler.compile_(bytecode, releaseType)) return true;
                        bytecode += (char) OPID::JF;
                        adr = bytecode.length();
                        bytecode += "0000";// 先占位
                    }
                    scCompiler.independent = true;
                    for (UL j = 0; j < ast.root->get(i + 1)->length(); j++) {
                        if (j == ast.root->get(i + 1)->length() - 1) scCompiler.isBlock = true;
                        scCompiler.ast.root = ast.root->get(i + 1)->get(j);
                        if (scCompiler.compile_(bytecode, releaseType, false, brTab, ctTab)) return true;
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
                scCompiler.regCount = regCount;
                scCompiler.regValue = regValue;
                scCompiler.regVar = regVar;
                scCompiler.stkOffset = stkOffset;
                scCompiler.ast.root = conAsts->get(0);
                if (scCompiler.compile_(bytecode, releaseType)) return true;
                auto loopVars = scCompiler.newVars;
                for (auto & loopVar : loopVars) {
                    sym.insert(std::pair<string, UL>(loopVar, stkOffset++));
                    if (~regCount) {
                        bytecode += (char) MOV_REG;
                        bytecode += transI32S_bin(stkOffset - 1);
                        bytecode += regCount;
                        scCompiler.regVar[loopVar] = regVar[loopVar] = regCount--;
                    }
                }
                scCompiler.regCount = regCount;
                auto conAdrS = transI32S_bin(bytecode.length());
                scCompiler.ast.root = conAsts->get(1);
                scCompiler.independent = false;
                scCompiler.compilingForLoopCon = true;
                if (scCompiler.compile_(bytecode, releaseType)) return true;
                bytecode += (char) JF;
                auto conFAdr = bytecode.length();
                bytecode += "0000";// 占位
                scCompiler.independent = true;
                std::list<UL> breakTab, continueTab;
                scCompiler.compilingForLoopCon = false;
                scCompiler.newVars.clear();
                for (UL i = 1; i < ast.root->length(); i++) {
                    if (i == ast.root->length() - 1) scCompiler.isBlock = true;
                    scCompiler.ast.root = ast.root->get(i);
                    if (scCompiler.compile_(bytecode, releaseType, false, &breakTab, &continueTab)) return true;
                }
                scCompiler.isBlock = false;
//                bytecode += "\x06";
                auto lastConAdrS = transI32S_bin(bytecode.length());
                scCompiler.ast.root = conAsts->get(2);
                scCompiler.compilingForLoopUpd = true;
                if (scCompiler.compile_(bytecode, releaseType)) return true;
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
            case Lexer::GLOBAL_TOKEN: {
                if (!parent) {
                    std::cerr << "BLE400: Unexpected global flag at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
                for (UL i = 0; i < ast.root->length(); i++) {
                    if (!parent->has(ast.root->get(i)->value())) {
                        std::cerr << "BLE401: Unexpected global symbol '" << ast.root->get(i)->value() << "' at <" << filename << ">:" << ast.line() << std::endl;
                        return true;
                    }
                    if (needOut[ast.root->get(i)->value()]) continue;
                    sym[ast.root->get(i)->value()] = stkOffset++;
                    bytecode += (char) PUSH_OUTER;
                    bytecode += transI32S_bin(outerUsingList.size());
                    needOut[ast.root->get(i)->value()] = true;
                    outerUsingList.push_back(parent->get(ast.root->get(i)->value()));
                }
                break;
            }
            case Lexer::UNKNOWN_TOKEN: {
                if (independent) goto FINISH_A_COM;
                if (ast.root->value() == "true") {
                    bytecode += (char) PUSH_TRUE;
                    goto FINISH_A_COM;
                } else if (ast.root->value() == "false") {
                    bytecode += (char) PUSH_FALSE;
                    goto FINISH_A_COM;
                }
                bool isNotFun = true;
                auto oi = sym.find(ast.root->value());
                if (oi == sym.end()) {
                    auto om = macro.find(ast.root->value());
                    if (om == macro.end()) {
                        auto of = functionAdrTab.find(ast.root->value());
                        if (of == functionAdrTab.end()) {
                            std::cerr << "BLE100: Undefined symbol '" << ast.root->value() << "' at <" << filename << ">:" << ast.line() << std::endl;
                            return true;
                        } else {
                            isNotFun = false;
                            bytecode += (char) PUSH_FUN;
                            bytecode += transI32S_bin(of->second);
                        }
                    } else {
                        auto val = om->second;
                        Compiler compiler(filename, val);
                        COM_COM_SET_NOW_LINE(compiler);
                        compiler.sym = sym;
                        compiler.regCount = regCount;
                        compiler.regValue = regValue;
                        compiler.regVar = regVar;
                        compiler.functionAdrTab = functionAdrTab;
                        compiler.macro = macro;
                        compiler.independent = false;
                        if (compiler.compile_(bytecode, releaseType)) return true;
                        break;
                    }
                }
                auto offset = oi->second;
                if (inOPTOEXPR) bytecode += (char)OPID::REFER + transI32S_bin(offset);
                else if (isNotFun) bytecode += (char)OPID::PUSH + transI32S_bin(offset);
                break;
            }
            case Lexer::OBJECT_TOKEN:
            {
                bytecode += (char) PUSH_OBJ;
                for (UL i = 0; i < ast.root->length(); i += 2) {
                    string attr_name(ast.root->get(i)->value());
                    Compiler compiler(filename);
                    COM_COM_SET_NOW_LINE(compiler);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.sym = sym;
                    compiler.regCount = regCount;
                    compiler.regValue = regValue;
                    compiler.regVar = regVar;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.ast.root = ast.root->get(i + 1);
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin(attr_name.length()) + attr_name;
                    if (compiler.compile_(bytecode, releaseType)) return true;
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
                COM_COM_SET_NOW_LINE(compiler);
                compiler.ast.root = dotNode;
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.sym = sym;
                compiler.regCount = regCount;
                compiler.regValue = regValue;
                compiler.regVar = regVar;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                if (compiler.compile_(bytecode, releaseType)) return true;
                auto attrLast = --attrs.end();
                for (auto i = attrs.begin(); i != attrLast; i++) {
                    bytecode += (char) GET_ATTR_STR;
                    bytecode += transI32S_bin((*i).length()) + *i;
                }
                if (parentIsSet) {
                    bytecode += (char) PUSH_STR;
                } else {
                    if (inOPTOEXPR) bytecode += (char) GET_ATTR_STR;
                    else bytecode += (char) GET_CP_ATTR_STR;
                }
                bytecode += transI32S_bin((*attrLast).length()) + *attrLast;
                break;
            }
            case Lexer::LET_TOKEN: {
                for (UL i = 0; i < ast.root->length(); i += 2) {
                    if (ast.root->get(i + 1)->type() == Lexer::PN_IREFER_TOKEN) {
                        macro[ast.root->get(i)->value()] = ast.root->get(i + 1)->get(0)->value();
                        continue;
                    }
                    Compiler compiler(filename);
                    COM_COM_SET_NOW_LINE(compiler);
                    compiler.ast.child = true;
                    compiler.independent = false;
                    compiler.ast.root = ast.root->get(i + 1)->get(0);
                    compiler.sym = sym;
                    compiler.regCount = regCount;
                    compiler.regValue = regValue;
                    compiler.regVar = regVar;
                    compiler.functionAdrTab = functionAdrTab;
                    compiler.macro = macro;
                    compiler.stkOffset = stkOffset;
                    if (compiler.compile_(bytecode, releaseType, ast.root->get(i + 1)->type() == Lexer::PN_DREFER_TOKEN)) return true;
                    newVars.push_back(ast.root->get(i)->value());
                    sym[ast.root->get(i)->value()] = stkOffset++;
                }
                break;
            }
            case Lexer::WHILE_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.stkOffset = stkOffset;
                scCompiler.regCount = regCount;
                scCompiler.regValue = regValue;
                scCompiler.regVar = regVar;
                scCompiler.ast.child = true;
                scCompiler.independent = false;
                scCompiler.ast.root = ast.root->get(0);
                UL conAdr = bytecode.length();
                if (scCompiler.compile_(bytecode, releaseType)) return true;
                bytecode += (char) OPID::JF;
                UL JLastAdr = bytecode.length();
                bytecode += "0000";// 先占位
                scCompiler.independent = true;
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    if (i == ast.root->length() - 1) scCompiler.isBlock = true;
                    scCompiler.ast.root = ast.root->get(i);
                    if (scCompiler.compile_(bytecode, releaseType, false, &breakTab, &continueTab)) return true;
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
                break;
            }
            case Lexer::IMPORT_TOKEN:
            {
                if (!isRoot) {
                    std::cerr << "BLE302: You cannot import modules or libraries in a sub block at <" << filename << ">:" << ast.line() << std::endl;
                    return true;
                }
                string name(ast.root->get(0)->value());
                string path(ast.root->value());
                string moduleScript;
                if (readFile(path + ".bel", moduleScript)) {// 说明是拓展包
                    path += "/lib" + name + ".belib";
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin(path.length());
                    bytecode += path;
                    bytecode += (char) LOAD;
                    sym[name] = stkOffset++;
                    newVars.push_back(name);
                } else {
                    Compiler compiler(name, moduleScript);
                    COM_COM_SET_NOW_LINE(compiler);
                    compiler.isEntry = false;
                    string moduleBcString;
                    if ((*compiled)[name]) goto FINISH_A_COM;
                    compiler.compiled = compiled;
                    compiler.compile(moduleBcString);
                    writeFile(path + ".belc", moduleBcString);
                    bytecode += (char) PUSH_STR;
                    bytecode += transI32S_bin(path.length());
                    bytecode += path;
                    bytecode += (char) IMP;
                    sym[name] = stkOffset++;
                    newVars.push_back(name);
                }
                break;
            }
            case Lexer::DEBUGGER_TOKEN:
            {
                bytecode += (char) DEB;
                break;
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
                break;
            }
            case Lexer::DO_TOKEN: {
                Compiler scCompiler(filename);
                scCompiler.sym = sym;
                scCompiler.functionAdrTab = functionAdrTab;
                scCompiler.macro = macro;
                scCompiler.regCount = regCount;
                scCompiler.regValue = regValue;
                scCompiler.regVar = regVar;
                scCompiler.stkOffset = stkOffset;
                scCompiler.ast.child = true;
                UL bodyAdr = bytecode.length();
                std::list<UL> breakTab, continueTab;
                for (UL i = 1; i < ast.root->length(); i++) {
                    if (i == ast.root->length() - 1) scCompiler.isBlock = true;
                    scCompiler.ast.root = ast.root->get(i);
                    if (scCompiler.compile_(bytecode, releaseType, false, &breakTab, &continueTab)) return true;
                }
                scCompiler.independent = false;
                scCompiler.isBlock = false;
                scCompiler.ast.root = ast.root->get(0);
                auto conAdrS = transI32S_bin(bytecode.length());
                if (scCompiler.compile_(bytecode, releaseType)) return true;
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
                COM_COM_SET_NOW_LINE(compiler);
                compiler.ast.child = true;
                compiler.independent = false;
                compiler.ast.root = ast.root->get(0);
                compiler.sym = sym;
                compiler.regCount = regCount;
                compiler.regValue = regValue;
                compiler.regVar = regVar;
                compiler.functionAdrTab = functionAdrTab;
                compiler.macro = macro;
                compiler.stkOffset = stkOffset;
                if (ast.root->type() == Lexer::BRACKETS_LEFT_TOKEN) {
                    auto oi = functionAdrTab.find(ast.root->get(0)->value());
                    UL index = -1;
                    if (oi != functionAdrTab.end()) {
                        index = oi->second;
                    }
                    for (UL i = 1; i < ast.root->length(); i++) {
                        compiler.ast.root = ast.root->get(i);
                        if (compiler.compile_(bytecode, releaseType)) return true;
                    }
                    if (~index) {
                        bytecode += (char) NEW_FRAME_AND_CALL;
                        bytecode += transI32S_bin(ast.root->length() - 1);
                        bytecode += transI32S_bin(index);
                    } else {
                        compiler.ast.root = ast.root->get(0);
                        if (compiler.compile_(bytecode, releaseType)) return true;
                        bytecode += (char) NEW_FRAME_AND_CALL_AND_CALL_FUN;
                        bytecode += transI32S_bin(ast.root->length() - 1);
                    }
                    if (independent)
                        bytecode += (char)OPID::POP;
                    break;
                } else if (ast.root->type() == Lexer::SET_TOKEN) {
                    compiler.parentIsSet = true;
                }
                if (compilingForLoopCon) {
                    if (ast.root->length() > 1 && ast.root->type() > Lexer::COLON_TOKEN && ast.root->type() < Lexer::BRACKETS_LEFT_TOKEN && ast.root->get(0)->type() == Lexer::UNKNOWN_TOKEN && ast.root->get(1)->type() == Lexer::NUMBER_TOKEN && regVar.find(ast.root->get(0)->value()) != regVar.end()) {
#define COM_EXPR_CFLC_CASE(NAME) case Lexer::NAME##_TOKEN: bytecode += (char) REG_##NAME; break;
                        switch (ast.root->type()) {
                            COM_EXPR_CFLC_CASE(MORE)
                            COM_EXPR_CFLC_CASE(LESS)
                            COM_EXPR_CFLC_CASE(EQ)
                            COM_EXPR_CFLC_CASE(NEQ)
                            COM_EXPR_CFLC_CASE(LEQ)
                            COM_EXPR_CFLC_CASE(MEQ)
                        }
                        bytecode += regVar[ast.root->get(0)->value()];
                        bytecode += transI64S_bin(transDI64_bin(transSD(ast.root->get(1)->value())));
                        goto FINISH_A_COM;
                    }
                } else if (compilingForLoopUpd) {
                    if ((ast.root->type() == Lexer::DADD_TOKEN || ast.root->type() == Lexer::DSUB_TOKEN) && !ast.root->get(0)->length() && regVar.find(ast.root->get(0)->value()) != regVar.end()) {
                        if (ast.root->type() == Lexer::DADD_TOKEN) bytecode += (char) REG_ADD;
                        else bytecode += (char) REG_SUB;
                        bytecode += regVar[ast.root->get(0)->value()];
                        bytecode += transI64S_bin(transDI64_bin(1));
                        goto FINISH_A_COM;
                    }
                    if ((ast.root->type() == Lexer::ADD_TO_TOKEN || ast.root->type() == Lexer::SUB_TO_TOKEN) && ast.root->length() == 2 && ast.root->get(0)->type() == Lexer::UNKNOWN_TOKEN && ast.root->get(1)->type() == Lexer::NUMBER_TOKEN && regVar.find(ast.root->get(0)->value()) != regVar.end()) {
                        if (ast.root->type() == Lexer::ADD_TO_TOKEN) bytecode += (char) REG_ADD;
                        else bytecode += (char) REG_SUB;
                        bytecode += regVar[ast.root->get(0)->value()];
                        bytecode += transI64S_bin(transDI64_bin(transSD(ast.root->get(1)->value())));
                        goto FINISH_A_COM;
                    }
                }
                if (compiler.compile_(bytecode, releaseType, ast.root->type() > Lexer::SRIGHT_TOKEN && ast.root->type() < Lexer::IN_TOKEN))
                    return true;
                if (ast.root->length() == 2) {
                    compiler.parentIsSet = false;
                    compiler.ast.root = ast.root->get(1);
                    if (compiler.compile_(bytecode, releaseType)) return true;
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
                    case Lexer::EQ_TOKEN:
                        bytecode += (char)OPID::EQ;
                        break;
                    case Lexer::NEQ_TOKEN:
                        bytecode += (char)OPID::NEQ;
                        break;
                    case Lexer::LEQ_TOKEN:
                        bytecode += (char)OPID::LEQ;
                        break;
                    case Lexer::MEQ_TOKEN:
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
                        if (ast.root->get(0)->type() == Lexer::DOT_TOKEN || ast.root->get(0)->type() == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN)
                            bytecode += (char)OPID::SET_ATTR;
                        else
                            bytecode += (char)OPID::MOV;
                        break;
                    case Lexer::DADD_TOKEN:
                    case Lexer::DSUB_TOKEN:
                        if (independent) {
                            bytecode += (char)OPID::PUSH_NUM;
                            bytecode += transI64S_bin(transDI64_bin(1));
                            if (ast.root->type() == Lexer::DADD_TOKEN) bytecode += (char)OPID::ADD;
                            else bytecode += (char)OPID::SUB;
                        } else {
                            bytecode += (char)OPID::SAV;
                            bytecode += (char)OPID::PUSH_NUM;
                            bytecode += transI64S_bin(transDI64_bin(1));
                            if (ast.root->type() == Lexer::DADD_TOKEN) bytecode += (char)OPID::ADD;
                            else bytecode += (char)OPID::SUB;
                            bytecode += (char)OPID::POP;
                            bytecode += (char)OPID::BAC;
                        }
                        break;
                }
                if (independent)
                    bytecode += (char)OPID::POP;
                break;
            }
        }
        FINISH_A_COM:
        if (ast.child) break;
    }
    if (isBlock || isRoot) {
        bytecode += (char) POPC;
        bytecode += transI32S_bin(newVars.size());
    }
    footerAdr = bytecode.length();
    if (!isRoot) return false;
    bytecode += transI32S_bin(functionAsts.size());
    for (UL i = 0; i < functionAsts.size(); i++) bytecode += "0000";
    UL j = 0;
    for (auto i = functionAsts.begin(); i != functionAsts.end(); i++, j++) {
        auto adrS = transI32S_bin(bytecode.length());
        bytecode[footerAdr + j * 4 + 4] = adrS[0];
        bytecode[footerAdr + j * 4 + 5] = adrS[1];
        bytecode[footerAdr + j * 4 + 6] = adrS[2];
        bytecode[footerAdr + j * 4 + 7] = adrS[3];
        ast.root = *i;
        Compiler compiler(filename);
        compiler.parent = this;
        compiler.ast.child = true;
        compiler.argCount = ast.root->get(0)->length();
        compiler.funName = ast.root->value();
        compiler.macro = macro;
        compiler.regCount = regCount;
        compiler.regValue = regValue;
        compiler.regVar = regVar;
        compiler.functionAdrTab = functionAdrTab;
        compiler.funStart = bytecode.length();
        for (auto k = 0; k < ast.root->get(0)->length(); k++)
            compiler.sym[ast.root->get(0)->get(k)->value()] = compiler.stkOffset++;
        UL footerAdrStoreAdr = bytecode.length();
        bytecode += "0000";
        bytecode += (char) RESIZE;
        bytecode += transI32S_bin(ast.root->get(0)->length());// 参数多了就删，少了就补
        for (auto k = 1; k < ast.root->length(); k++) {
            compiler.ast.root = ast.root->get(k);
            if (compiler.compile_(bytecode, releaseType)) {
                std::cerr << "\t at <" << filename << ">:" << ast.line() << std::endl;
                return true;
            }
        }
        bytecode += (char) PUSH_UND;
        bytecode += (char) RET;
        string footerAdrStr(transI32S_bin(bytecode.length()));
        bytecode[footerAdrStoreAdr] = footerAdrStr[0];
        bytecode[footerAdrStoreAdr + 1] = footerAdrStr[1];
        bytecode[footerAdrStoreAdr + 2] = footerAdrStr[2];
        bytecode[footerAdrStoreAdr + 3] = footerAdrStr[3];
        bytecode += transI32S_bin(compiler.outerUsingList.size());
        for (auto & iter : compiler.outerUsingList) {
            bytecode += transI32S_bin(iter);
        }
    }
    return false;
}