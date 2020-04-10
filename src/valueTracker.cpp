#include "valueTracker.h"

void Belish::ValueTracker::track(AST::node* node, bool isGettingLvalue) {
    switch (node->type()) {
        case Lexer::LET_TOKEN: {
            for (UL i = 0; i < node->length(); i += 2) {
                track(node->get(i + 1)->get(0));
                auto value = node->get(i + 1)->get(0)->nodevalue;
                values[node->get(i)->value()] = value;
                value->bind();
            }
            node->nodevalue = new Unknown;
            break;
        }
        case Lexer::NUMBER_TOKEN:
        {
            node->nodevalue = new Number(atof(node->v.c_str()));
            break;
        }
        case Lexer::STRING_TOKEN:
        {
            node->nodevalue = new String(node->v);
            break;
        }
        case Lexer::UNKNOWN_TOKEN:
        {
            if (node->v == "true") {
                node->nodevalue = new Boolean(true);
                break;
            } else if (node->v == "false") {
                node->nodevalue = new Boolean(false);
                break;
            }
            node->nodevalue = values[node->v];
            break;
        }
#define VALUE_TRACKER_TRACK_SAME_TYPE node->get(0)->nodevalue->type() == node->get(1)->nodevalue->type()
        // ========符号运算操作==========
        case Lexer::SET_TOKEN:
        {
            isGettingLvalue = true;
            track(node->get(0), true);
            track(node->get(1));
            node->nodevalue = node->get(0)->nodevalue;
            if (VALUE_TRACKER_TRACK_SAME_TYPE) node->nodevalue->set(node->get(1)->nodevalue);
            else {
                auto vLinked = node->nodevalue->linked;
                switch (node->nodevalue->type()) {
                    case NUMBER: ((Number*)node->nodevalue)->~Number(); break;
                    case STRING: ((String*)node->nodevalue)->~String(); break;
                    case INT: ((Int*)node->nodevalue)->~Int(); break;
                    case UNDEFINED: ((Undefined*)node->nodevalue)->~Undefined(); break;
                    case OBJECT: ((Object*)node->nodevalue)->~Object(); break;
                    case FUNCTION: ((Function*)node->nodevalue)->~Function(); break;
                    case ARRAY: ((Array*)node->nodevalue)->~Array(); break;
                }
                switch (node->get(1)->nodevalue->type()) {
                    case NUMBER: node->nodevalue = new (node->nodevalue)Number; break;
                    case STRING: node->nodevalue = new (node->nodevalue)String; break;
                    case INT: node->nodevalue = new (node->nodevalue)Int; break;
                    case UNDEFINED: node->nodevalue = new (node->nodevalue)Undefined; break;
                    case OBJECT: node->nodevalue = new (node->nodevalue)Object; break;
                    case FUNCTION: node->nodevalue = new (node->nodevalue)Function; break;
                }
                node->nodevalue->linked = vLinked;
                node->nodevalue->set(node->get(1)->nodevalue);
            }
            break;
        }
#define VALUE_TRACKER_TRACK_SWITCH_ORD_OP(OP, op) \
        case Lexer::OP##_TOKEN: \
        { \
            track(node->get(0)); \
            track(node->get(1)); \
            if (VALUE_TRACKER_TRACK_SAME_TYPE) { \
                node->nodevalue = node->get(0)->nodevalue->copy(); \
                node->nodevalue->op(node->get(1)->nodevalue); \
            } else node->nodevalue = new Unknown; \
            break; \
        }
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(ADD, add)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(SUB, sub)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MUL, mul)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(DIV, div)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MOD, mod)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(POWER, pow)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(SLEFT, shiftl)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(SRIGHT, shiftr)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MAND, mand)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MOR, mor)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(LAND, land)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(LOR, lor)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(LESS, less)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MORE, more)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(EQ, eq)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(NEQ, neq)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(MEQ, meq)
        VALUE_TRACKER_TRACK_SWITCH_ORD_OP(LEQ, leq)
#define VALUE_TRACKER_TRACK_SWITCH_TO_OP(OP, op) \
        case Lexer::OP##_TO_TOKEN: \
        { \
            isGettingLvalue = true; \
            track(node->get(0), true); \
            track(node->get(1)); \
            if (VALUE_TRACKER_TRACK_SAME_TYPE) { \
                node->nodevalue = node->get(0)->nodevalue; \
                node->get(0)->nodevalue->op(node->get(1)->nodevalue); \
            } else node->nodevalue = new Unknown; \
            break; \
        }
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(ADD, add)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(SUB, sub)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(MUL, mul)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(DIV, div)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(MOD, mod)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(POWER, pow)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(SLEFT, shiftl)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(SRIGHT, shiftr)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(MAND, mand)
        VALUE_TRACKER_TRACK_SWITCH_TO_OP(MOR, mor)
        default:
            node->nodevalue = new Unknown;
            return;
    }
    node->nodevalue->bind();
    if (!isGettingLvalue && node->nodevalue) {
        if (node->nodevalue->type() == UNKNOWN) return;
        for (auto i = 0; i < node->length(); i++) {
            if (node->children[i]) delete node->children[i];
        }
        node->clear();
        switch (node->nodevalue->type()) {
#define VALUE_TRACKER_TRACK_OPT_AST_CASE(TYPE) case TYPE: node->type(Lexer::TYPE##_TOKEN);
            VALUE_TRACKER_TRACK_OPT_AST_CASE(NUMBER)
            {
                node->value(std::to_string(((Number*)node->nodevalue)->value()));
                break;
            }
            VALUE_TRACKER_TRACK_OPT_AST_CASE(STRING)
            {
                node->value(((String*)node->nodevalue)->value());
                break;
            }
        }
    }
}