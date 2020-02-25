#include <iostream>
#include <cstring>
#include "compiler.h"
#include "vm.h"
#include "trans.h"
#include "trans.cpp"
#include "values.h"
#include "fio.h"

void Belish::BVM::run() {
    Byte byte;
    Dbyte dbyte;
    Qbyte qbyte;
    Ebyte ebyte;
    Value* cache = nullptr;
    if (callMoudleMethod) goto CALL_MODULE_METHOED;
    i = 0;
    GETQBYTE
    if (qbyte != 0x9ad0755c) {
        std::cerr << "Wrong magic code -> " << ((void*)(i - 4)) << std::endl;
        return;
    }
    GETDBYTE
    GETEBYTE
    GETQBYTE
    i = footerAdr = qbyte;
    // 关于定义函数、类等的处理
    // 获取所有函数
    GETQBYTE
    functionLen = qbyte;
    functions.reserve(functionLen);
    for (UL j = 0; j < functionLen; j++) {
        GETQBYTE
        functions.push_back(qbyte);
    }
    if (!stk) {
        stk = new Stack;
        frames.push_back(stk);
    }
    for (i = 18; i < footerAdr || inFun; ) {
        CALL_MODULE_METHOED:
        GETBYTE;
        auto op = (OPID)byte;
//        if (stk->length() < 2) {
//            std::cerr << "Cannot perform binocular operation" << std::endl;
//            return;
//        }
        auto a = stk->get(stk->length() - 2);
        auto b = stk->get(stk->length() - 1);
        switch (op) {
            case ADD: {
//                stk->dbg();
                if (a->type() == b->type()) a->add(b);
                else { std::cerr << "Wrong type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case SL: {
                if (b->type() == NUMBER) a->shiftl(b);
                else { std::cerr << "Wrong type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case SR: {
                if (b->type() == NUMBER) a->shiftr(b);
                else { std::cerr << "Wrong type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case SUB: {
                if (a->type() == b->type()) a->sub(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MUL: {
                if (a->type() == b->type() || (b->type() == NUMBER && a->type() == STRING)) a->mul(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case DIV: {
                if (a->type() == b->type()) a->div(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MOD: {
                if (a->type() == b->type()) a->mod(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case EQ: {
                if (a->type() == b->type()) a->eq(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case NEQ: {
                if (a->type() == b->type()) a->neq(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case LEQ: {
                if (a->type() == b->type()) a->leq(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MEQ: {
                if (a->type() == b->type()) a->meq(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case LESS: {
                if (a->type() == b->type()) a->less(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MORE: {
                if (a->type() == b->type()) a->more(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MAND: {
                if (a->type() == b->type()) a->mand(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MOR: {
                if (a->type() == b->type()) a->mor(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case MXOR: {
                if (a->type() == b->type()) a->mxor(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case LAND: {
                if (a->type() == b->type()) a->land(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case LOR: {
                if (a->type() == b->type()) a->lor(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case POW: {
                if (a->type() == b->type()) a->pow(b);
                else { std::cerr << "Wrong  type -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                break;
            }
            case PUSH_NUM:
                GETEBYTE
                stk->push(new Number(transI64D_bin(ebyte)));
                break;
            case PUSH_STR: {
                GETQBYTE
                UL strlen = qbyte;
                string str;
                for (auto j = 0; j < strlen; j++, i++) {
                    str += bytecode[i];
                }
                stk->push(new String(str));
                break;
            }
            case PUSH_UND: {
                stk->push(new Undefined);
                break;
            }
            case REFER: {
                GETQBYTE
                stk->push(stk->get(qbyte));
                break;
            }
            case PUSH: {
                GETQBYTE
                auto val = stk->get(qbyte);
                if (val->type() == OBJECT) stk->push(val);
                else stk->push(val->copy());
                break;
            }
            case POP: {
                stk->pop(1);
//                stk->dbg();
                break;
            }
            case POPC: {
                GETQBYTE
                stk->pop(qbyte);
                break;
            }
            case JT: {
                GETQBYTE
                if (stk->top()->isTrue()) i = qbyte;
                stk->pop(1);
                break;
            }
            case JF: {
                GETQBYTE
                if (stk->top()->isFalse()) i = qbyte;
                stk->pop(1);
                break;
            }
            case JMP: {
                GETQBYTE
                i = qbyte;
                break;
            }
            case CHANGE: {
                GETQBYTE
                stk->set(qbyte, stk->top());
                stk->pop(1);
                break;
            }
            case PUSH_FUN: {
                GETQBYTE
                stk->push(new Function(qbyte));
                break;
            }
            case SAV: {
                cache = stk->top()->copy();
                break;
            }
            case BAC: {
                stk->push(cache);
                break;
            }
            case PUSH_TRUE: {
                stk->push(new Boolean(true));
                break;
            }
            case PUSH_FALSE: {
                stk->push(new Boolean(false));
                break;
            }
            case LNOT: {
                stk->top()->lnot();
                break;
            }
            case MNOT: {
                stk->top()->mnot();
                break;
            }
            case MOV: {
                auto v = stk->get(stk->length() - 2);
                auto t = stk->top();
                if (stk->top()->type() == v->type()) v->set(t);
                else {
                    auto vLinked = v->linked;
                    switch (v->type()) {
                        case NUMBER: ((Number*)v)->~Number(); break;
                        case STRING: ((String*)v)->~String(); break;
                        case INT: ((Int*)v)->~Int(); break;
                        case UNDEFINED: ((Undefined*)v)->~Undefined(); break;
                        case OBJECT: ((Object*)v)->~Object(); break;
                        case FUNCTION: ((Function*)v)->~Function(); break;
                    }
                    switch (stk->top()->type()) {
                        case NUMBER: v = new (v)Number; break;
                        case STRING: v = new (v)String; break;
                        case INT: v = new (v)Int; break;
                        case UNDEFINED: v = new (v)Undefined; break;
                        case OBJECT: v = new (v)Object; break;
                        case FUNCTION: v = new (v)Function; break;
                    }
                    v->linked = vLinked;
                    v->set(t);
                }
                stk->pop(1);
                break;
            }
            case PUSH_OBJ: {
                stk->push(new Object);
                break;
            }
            case LOAD: {
                string path(((String*)stk->top())->value());
                auto exlib = new Dylib(path);
                if (!exlib->load()) {
                    std::cerr << "Failed to load -> " << ((void*)(i - 1)) << std::endl;
                    return;
                }
                auto moduleSetup = (ModuleSetup) exlib->resolve("moduleSetup");
                stk->pop(1);
                stk->push(moduleSetup());
                exlibs.push_back(exlib);
                break;
            }
            case IMP: {
                string path(((String*)stk->top())->value());
                ULL length;
                auto buffer = Belish::readFileCPTR(path + ".belc", length);
                auto vm = new BVM(buffer, length);
                vm->child = true;
                vm->run();
                modules.push_back(vm);
                auto __exports__ = (Object*)vm->stk->get(0);// 偏移值为0的即__exports__变量
                for (auto iter = __exports__->begin(); ; ) {
                    auto val = iter.value();
                    if (val->type() == FUNCTION) ((Function*)val)->module = modules.size();
                    if (!iter.next()) break;
                }
                stk->pop(1);
                stk->push(__exports__);
                break;
            }
            case SET_ATTR: {
                string attr_name(((String*)stk->get(stk->length() - 2))->value());
                auto obj_ = stk->get(stk->length() - 3);
                if (obj_->type() != OBJECT) { std::cerr << "Wrong type to set attr -> " << ((void*)(i - 1)) << std::endl; return; }
                auto obj = (Object*)obj_;
                obj->set(attr_name, stk->top());
                stk->pop(1);
                stk->pop(1);
                break;
            }
            case GET_ATTR: {
                string attr_name(((String*)stk->top())->value());
                auto obj_ = stk->get(stk->length() - 2);
                if (obj_->type() != OBJECT) { std::cerr << "Wrong type to get attr -> " << ((void*)(i - 1)) << std::endl; return; }
                stk->pop(1);
                stk->pop(1);
                auto obj = (Object*)obj_;
                auto attr = obj->get(attr_name);
                if (attr) {
                    stk->push(attr);
                } else {
                    attr = new Undefined;
                    obj->set(attr_name, attr);
                    stk->push(attr);
                }
                break;
            }
            case NEW_FRAME: {
                GETQBYTE
                UL movCount(qbyte);
                auto stk_ = stk;
                stk = new Stack;
                frames.push_back(stk);
                for (UL j = movCount; j; j--) stk->push(stk_->get(stk_->length() - j));
                stk_->pop(movCount);
                break;
            }
            case RESIZE: {
                GETQBYTE
                UL size(qbyte);
                if (stk->length() >= size) stk->pop(stk->length() - size);
                else for (UL j = 0; j < size - stk->length(); j++) stk->push(new Undefined);
                break;
            }
            case CALL: {
                GETQBYTE
                UL funIndex(qbyte);
                if (funIndex >= functions.size()) { std::cerr << "Exceeded expected function index value -> " << ((void*)(i - 5)) << std::endl; return; }
                frames[frames.size() - 2]->push(new Int(i));
                i = functions[funIndex];
                inFun++;
                break;
            }
            case CALL_FUN: {
                if (stk->top()->type() == NFUNCTION) {
                    auto nfun = ((NFunction*)stk->top());
                    stk->pop(1);
                    auto ret = nfun->call(stk);
                    delete stk;
                    frames.erase(frames.end() - 1);
                    stk = frames[frames.size() - 1];
                    stk->push(ret);
                    break;
                }
                auto fun = ((Function*)stk->top());
                UL funIndex(fun->id());
                if (fun->module) {// 不为0
                    auto vm = modules[fun->module - 1];
                    if (funIndex >= vm->functions.size()) { std::cerr << "Exceeded expected function index value -> " << ((void*)(i - 1)) << std::endl; return; }
                    vm->i = vm->functions[funIndex];
                    vm->stk->push(new Int(-1));
                    vm->frames.push_back(stk);
                    vm->stk = vm->frames[vm->frames.size() - 1];
                    vm->inFun++;
                    vm->callMoudleMethod = true;
                    stk = frames[frames.size() - 2];
                    frames.erase(frames.end() - 1);
                    vm->run();
                    stk->push(vm->stk->top());
                    break;
                }
                if (funIndex >= functions.size()) { std::cerr << "Exceeded expected function index value -> " << ((void*)(i - 1)) << std::endl; return; }
                frames[frames.size() - 2]->push(new Int(i));
                i = functions[funIndex];
                inFun++;
                stk->pop(1);
                break;
            }
            case RET: {
                auto ret = stk->top();
                ret->linked++;
                delete stk;
                ret->linked--;
                frames.erase(frames.end() - 1);
                stk = frames[frames.size() - 1];
                i = ((Int*)stk->top())->value();
                stk->pop(1);
                stk->push(ret);
                inFun--;
                if (i == -1) return;
                break;
            }
            case REG_EQ:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() == transI64D_bin(ebyte)));
                break;
            }
            case REG_NEQ:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() != transI64D_bin(ebyte)));
                break;
            }
            case REG_MEQ:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() >= transI64D_bin(ebyte)));
                break;
            }
            case REG_LEQ:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() <= transI64D_bin(ebyte)));
                break;
            }
            case REG_MORE:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() > transI64D_bin(ebyte)));
                break;
            }
            case REG_LESS:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                stk->push(new Boolean(((Number*)regs[reg])->value() < transI64D_bin(ebyte)));
                break;
            }
            case REG_ADD:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                ((Number*)regs[reg])->value() += transI64D_bin(ebyte);
                break;
            }
            case REG_SUB:
            {
                GETBYTE;
                auto reg = byte;
                GETEBYTE
                ((Number*)regs[reg])->value() -= transI64D_bin(ebyte);
                break;
            }
            case MOV_REG:
            {
                GETQBYTE
                GETBYTE;
                regs[byte] = stk->get(qbyte);
                break;
            }
            case DEB:
                stk->dbg();
                break;
        }
    }
    // 测试
//    stk->dbg();
    if (!child) {
        for (auto & module : modules) {
            delete module->bytecode;
            delete module;
        }
        for (auto & exlib : exlibs) {
            delete exlib;
        }
        delete stk;
        stk = nullptr;
    }
}
