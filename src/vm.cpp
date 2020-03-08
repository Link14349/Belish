#include <iostream>
#include <list>
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
    Value* preValue = nullptr;// 存储取属性运算符.(也可以是[])左边的值
    vector<vector<UL> > outers;
    vector<vector<Value*> > outersDefs;
    vector<Value*>* closure = nullptr;
    if (importedTab) (*importedTab)[filename] = true;
    else importedTab = new map<string, bool>;
    if (callModuleMethod) goto CALL_MODULE_METHOED;
    callingLineStk.push_back(1);
    i = 0;
    (*importedTab)[filename] = true;
    // get magic code
    GETQBYTE
    if (qbyte != 0x9ad0755c) {
        Throw(500, "Wrong magic code");
        return;
    }
    // get main version&sub version
    GETDBYTE
    // get compiling timestamp
    GETEBYTE
    // get footer adress
    GETQBYTE
    i = footerAdr = qbyte;
    // 关于定义函数、类等的处理
    // 获取所有函数
    GETQBYTE
    functionLen = qbyte;
    functions.reserve(functionLen);
    outers.reserve(functionLen);
    outersDefs.reserve(functionLen);
    for (UL j = 0; j < functionLen; j++) {
        GETQBYTE
        functions.push_back(qbyte + 4);
        outers.emplace_back();
        outersDefs.emplace_back();
        UL tmpI = i;
        i = qbyte;
        GETQBYTE
        i = qbyte;
        GETQBYTE
        UL outerLen = qbyte;
        outers.reserve(outerLen);
        outersDefs.reserve(outerLen);
        for (UL k = 0; k < outerLen; k++) {
            GETQBYTE
            outers.back().push_back(qbyte);
        }
        i = tmpI;
    }
    if (!stk) {
        stk = new Stack(objects, deathObjects);
        frames.push_back(stk);
    }
    for (i = 18; i < footerAdr || inFun; ) {
        CALL_MODULE_METHOED:
        GETBYTE;
        auto op = (OPID)byte;
        auto a = stk->get(stk->length() - 2);
        auto b = stk->get(stk->length() - 1);
        switch (op) {
            case LINE: {
                GETQBYTE
                callingLineStk.back() = qbyte;
                break;
            }
            case PUSH_OUTER: {
//                stk->dbg();
                if (!closure) { Throw(600, "Isn't in a function calling"); return; }
                GETQBYTE
                stk->push((*closure)[qbyte]);
                break;
            }
            case ADD: {
//                stk->dbg();
                if (a->type() == b->type()) a->add(b);
                else { Throw(501, "Wrong type to add"); return; }
                stk->pop(1);
                break;
            }
            case SL: {
                if (b->type() == NUMBER) a->shiftl(b);
                else { Throw(501, "Wrong type to shift left"); return; }
                stk->pop(1);
                break;
            }
            case SR: {
                if (b->type() == NUMBER) a->shiftr(b);
                else { Throw(501, "Wrong type to shift right"); return; }
                stk->pop(1);
                break;
            }
            case SUB: {
                if (a->type() == b->type()) a->sub(b);
                else { Throw(501, "Wrong type to sub"); return; }
                stk->pop(1);
                break;
            }
            case MUL: {
                if (a->type() == b->type() || (b->type() == NUMBER && a->type() == STRING)) a->mul(b);
                else { Throw(501, "Wrong type to mul"); return; }
                stk->pop(1);
                break;
            }
            case DIV: {
                if (a->type() == b->type()) a->div(b);
                else { Throw(501, "Wrong type to div"); return; }
                stk->pop(1);
                break;
            }
            case MOD: {
                if (a->type() == b->type()) a->mod(b);
                else { Throw(501, "Wrong type to mod"); return; }
                stk->pop(1);
                break;
            }
            case EQ: {
                if (a->type() == b->type()) a->eq(b);
                else { Throw(501, "Wrong type to compare equal"); return; }
                stk->pop(1);
                break;
            }
            case NEQ: {
                if (a->type() == b->type()) a->neq(b);
                else { Throw(501, "Wrong type to compare not equal"); return; }
                stk->pop(1);
                break;
            }
            case LEQ: {
                if (a->type() == b->type()) a->leq(b);
                else { Throw(501, "Wrong type to compare less or equal"); return; }
                stk->pop(1);
                break;
            }
            case MEQ: {
                if (a->type() == b->type()) a->meq(b);
                else { Throw(501, "Wrong type to compare more or equal"); return; }
                stk->pop(1);
                break;
            }
            case LESS: {
                if (a->type() == b->type()) a->less(b);
                else { Throw(501, "Wrong type to compare less"); return; }
                stk->pop(1);
                break;
            }
            case MORE: {
                if (a->type() == b->type()) a->more(b);
                else { Throw(501, "Wrong type to compare more"); return; }
                stk->pop(1);
                break;
            }
            case MAND: {
                if (a->type() == b->type()) a->mand(b);
                else { Throw(501, "Wrong type to &"); return; }
                stk->pop(1);
                break;
            }
            case MOR: {
                if (a->type() == b->type()) a->mor(b);
                else { Throw(501, "Wrong type to |"); return; }
                stk->pop(1);
                break;
            }
            case MXOR: {
                if (a->type() == b->type()) a->mxor(b);
                else { Throw(501, "Wrong type to ^"); return; }
                stk->pop(1);
                break;
            }
            case LAND: {
                if (a->type() == b->type()) a->land(b);
                else { Throw(501, "Wrong type to &&"); return; }
                stk->pop(1);
                break;
            }
            case LOR: {
                if (a->type() == b->type()) a->lor(b);
                else { Throw(501, "Wrong type to ||"); return; }
                stk->pop(1);
                break;
            }
            case POW: {
                if (a->type() == b->type()) a->pow(b);
                else { Throw(501, "Wrong type to **"); return; }
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
                for (UL j = 0; j < strlen; j++, i++) {
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
                if (val->type() == OBJECT) {
                    stk->push(val);
                    objects[val]++;
                }
                else stk->push(val->copy());
                break;
            }
            case POP: {
                stk->pop(1);
//                stk->dbg();
                break;
            }
            case POPC: {// popc指令只会在一个块结束之际出现，所以在popc之后往往会产生一些垃圾
                GETQBYTE
                stk->pop(qbyte);
                if (deathObjects.size() > 4) gc.gc();
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
            case DEF_FUN: {
                GETQBYTE
                auto& funOuters = outers[qbyte];
                auto& funOutersDef = outersDefs[qbyte];
                for (unsigned int & funOuter : funOuters) {
                    auto v = stk->get(funOuter);
                    funOutersDef.push_back(v);
                    v->linked++;
                }
                break;
            }
            case DEF_FUN_AND_PUSH: {
                GETQBYTE
                auto& funOuters = outers[qbyte];
                auto& funOutersDef = outersDefs[qbyte];
                for (unsigned int & funOuter : funOuters) {
                    auto v = stk->get(funOuter);
                    funOutersDef.push_back(v);
                    v->linked++;
                }
                stk->push(new Function(qbyte));
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
                cache = nullptr;
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
                auto obj = new Object;
                stk->push(obj);
                objects[obj]++;
                break;
            }
            case LOAD: {
                string path(((String*)stk->top())->value());
                auto exlib = new Dylib(path);
                if (!exlib->load()) {
                    Throw(301, "Failed to load the library '" + path + "'");
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
                if ((*importedTab)[path]) break;// 已经导入过模块就不再导入了
                ULL length;
                auto buffer = Belish::readFileCPTR(path + ".belc", length);
                auto vm = new BVM(path, buffer, length);
                vm->importedTab = importedTab;
                vm->child = true;
                vm->run();
                if (vm->error) {
                    Throw(700, "The module that you are importing had some errors");
                    return;
                }
                modules.push_back(vm);
                auto __exports__ = (Object*)vm->stk->get(0);// 偏移值为0的即__exports__变量
                for (auto iter = __exports__->begin(); !iter.end(); iter.next()) {
                    auto val = iter.value();
                    if (val->type() == FUNCTION) ((Function*)val)->module = modules.size();
                }
                stk->pop(1);
                stk->push(__exports__);
                break;
            }
            case SET_ATTR: {
                string attr_name(((String*)stk->get(stk->length() - 2))->value());
                auto obj_ = stk->get(stk->length() - 3);
                if (obj_->type() != OBJECT) { Throw(800, "Wrong type to set attr"); return; }
                auto obj = (Object*)obj_;
                obj->set(attr_name, stk->top());
                stk->pop(1);
                stk->pop(1);
                break;
            }
            case GET_ATTR: {
                string attr_name(((String*)stk->top())->value());
                auto obj_ = stk->get(stk->length() - 2);
                if (obj_->type() != OBJECT) { Throw(801, "Wrong type to get attr"); return; }
                stk->pop(1);
                stk->pop(1);
                auto obj = (Object*)obj_;
                auto attr = obj->get(attr_name);
                if (attr) {
                    stk->push(attr);
                } else {
                    if (obj->get(CLASS_LINK)) {
                        attr = ((Object*)obj->get(CLASS_LINK))->get(attr_name);
                        if (attr) {
                            obj->set(attr_name, attr);
                            if (attr->type() == FUNCTION) ((Function*)attr)->isMethod = true;
                            stk->push(attr);
                            goto VM_GET_ATTR_LAST;
                        }
                    }
                    attr = new Undefined;
                    obj->set(attr_name, attr);
                    stk->push(attr);
                }
                VM_GET_ATTR_LAST:
                preValue = obj;
                break;
            }
            case NEW_FRAME: {
                GETQBYTE
                UL movCount(qbyte);
                auto stk_ = stk;
                stk = new Stack(objects, deathObjects);
                if (cache && cache->type() == FUNCTION && ((Function*)cache)->isMethod) {
                    stk->push(preValue);
                }
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
                if (funIndex >= functions.size()) { Throw(601, "Unknown function calling(wrong function index #" + std::to_string(qbyte) + ")"); return; }
                frames[frames.size() - 2]->push(new Int(i));
                i = functions[funIndex];
                closure = &outersDefs[funIndex];
                inFun++;
                callingLineStk.push_back(0);
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
                    if (funIndex >= vm->functions.size()) { Throw(601, "Unknown function calling(wrong function index #" + std::to_string(qbyte) + ")"); return; }
                    vm->i = vm->functions[funIndex];
                    vm->stk->push(new Int(-1));
                    vm->frames.push_back(stk);
                    vm->stk = vm->frames[vm->frames.size() - 1];
                    vm->inFun++;
                    vm->callModuleMethod = true;
                    stk = frames[frames.size() - 2];
                    frames.erase(frames.end() - 1);
                    vm->callingLineStk.push_back(0);
                    vm->run();
                    if (vm->error) {
                        Throw(701, "The module method that you are calling had some errors");
                        return;
                    }
                    stk->push(vm->stk->top());
                    break;
                }
                if (funIndex >= functions.size()) { Throw(601, "Unknown function calling(wrong function index #" + std::to_string(qbyte) + ")"); return; }
                frames[frames.size() - 2]->push(new Int(i));
                i = functions[funIndex];
                closure = &outersDefs[funIndex];
                inFun++;
                stk->pop(1);
                callingLineStk.push_back(0);
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
                callingLineStk.erase(--callingLineStk.end());
                if (deathObjects.size() > 4) gc.gc();// 函数调用完成后往往会产生一些垃圾
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
        gc.gc();
        for (auto & module : modules) {
            delete module->bytecode;
            module->gc.gc();
            delete module;
        }
        for (auto & exlib : exlibs) {
            delete exlib;
        }
        delete stk;
        stk = nullptr;
    }
}
