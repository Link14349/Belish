#include <climits>
#include <vector>
#include <list>
#include <regex>
#include "ast.h"

Belish::AST::~AST() {
    if (!child && root)
        delete root;
}

void Belish::AST::parse() {
    if (!child && root) delete root;
    root = nullptr;
#define GET token = lexer.get()
    auto initialLine = lexer.line();
    auto initialIndex = lexer.index();
    auto GET;
    switch (token.t) {
        case Lexer::NUMBER_TOKEN:
        case Lexer::STRING_TOKEN:
        case Lexer::BRACKETS_LEFT_TOKEN:
        case Lexer::UNKNOWN_TOKEN:
        case Lexer::UNDEFINED_TOKEN:
        case Lexer::NULL_TOKEN:
        {
            auto tmp_token = token;
            GET;
            if (token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN) {
                root = new node(tmp_token.t, tmp_token.s, lexer.line() + baseLine);
                break;
            }
            struct Operator {
                Operator() : priority(UINT_MAX) { }
                Lexer::Token token;
                UL index;
                UL line;
                ULL priority;
            } op;
            bool preTokenIsUnknown = tmp_token.t == Lexer::UNKNOWN_TOKEN;// included left brackets
            lexer.index(initialIndex);
            lexer.line(initialLine);
            ULL base = 1;
            GET;
            while (true) {
                if (token.t == Lexer::END_TOKEN || token.t == Lexer::PROGRAM_END) break;
                if (token.t > Lexer::NOTE_TOKEN) {
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN || token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        base *= 14;
                        if (preTokenIsUnknown) {// 说明是函数/取值符
                            if (base <= (ULL)op.priority) {
                                op.token = token;
                                op.line = lexer.line();
                                op.index = lexer.index();
                                op.priority = base;
                            }
                        }
                        preTokenIsUnknown = true;
                    } else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN || token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) {
                        preTokenIsUnknown = true;
                        base /= 14;
                    }
                    else {
                        preTokenIsUnknown = false;
                        if (base * (ULL)priority(token.t) <= (ULL)op.priority) {
                            op.token = token;
                            op.line = lexer.line();
                            op.index = lexer.index();
                            op.priority = base * (ULL)priority(token.t);
                        }
                    }
                } else preTokenIsUnknown = true;
                GET;
            }
            string left;
            U bracketsCount(0);
            auto lexerLine = lexer.line();
            lexer.index(initialIndex);
            lexer.line(initialLine);
            GET;
            while (lexer.index() < op.index) {
                left += token.s + " ";
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bracketsCount++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bracketsCount--;
                GET;
            }
            while (bracketsCount--) {
                left.erase(0, left.find('(') + 1);
            }
            root = new node(op.token.t, op.token.s, op.line + baseLine);
            if (root->type() == Lexer::BRACKETS_LEFT_TOKEN || root->type() == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                AST la(left, baseLine + initialIndex);
                la.parse();
                root->insert(la.root);

                lexer.index(op.index);
                string arg;
                bracketsCount = 1;
                while (true) {
                    GET;
                    if (token.t == Lexer::END_TOKEN || token.t == Lexer::PROGRAM_END) {
                        break;
                    }
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                        bracketsCount++;
                        arg += token.s + " ";
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                        bracketsCount--;
                        if (!bracketsCount) {
                            AST argA(arg, baseLine + initialIndex);
                            argA.parse();
                            root->insert(argA.root);
                            break;
                        }
                        arg += token.s + " ";
                    }
                    else if (token.t == Lexer::COMMA_TOKEN && bracketsCount == 1) {
                        AST argA(arg, baseLine + initialIndex);
                        argA.parse();
                        root->insert(argA.root);
                        arg = "";
                    } else arg += token.s + " ";
                }
                __asm("nop");
            } else {
                bracketsCount = 0;
                string right;
                lexer.index(op.index);
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += token.s + " ";
                    if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bracketsCount++;
                    else if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bracketsCount--;
                    GET;
                }
                while (bracketsCount--) {
                    auto tmp = right.rfind(')');
                    right.erase(right.length() - tmp - 1, tmp + 1);
                }
                AST la(left, baseLine + initialIndex);
                AST ra(right, baseLine + initialIndex);
                la.parse();
                ra.parse();
                root->insert(la.root);
                root->insert(ra.root);
            }
            lexer.line(lexerLine);
            break;
        }
    }
}

inline unsigned short Belish::AST::priority(Lexer::TOKENS& tk) {
    switch (tk) {
        case Lexer::BRACKETS_LEFT_TOKEN: return 14;
        case Lexer::DADD_TOKEN:
        case Lexer::DSUB_TOKEN:
        case Lexer::LNOT_TOKEN:
        case Lexer::MNOT_TOKEN:
        case Lexer::POWER_TOKEN:
            return 13;
        case Lexer::MUL_TOKEN:
        case Lexer::DIV_TOKEN:
        case Lexer::MOD_TOKEN:
            return 12;
        case Lexer::ADD_TOKEN:
        case Lexer::SUB_TOKEN:
            return 11;
        case Lexer::SLEFT_TOKEN:
        case Lexer::SRIGHT_TOKEN:
            return 10;
        case Lexer::MORE_TOKEN:
        case Lexer::MEQUAL_TOKEN:
        case Lexer::LESS_TOKEN:
        case Lexer::LEQUAL_TOKEN:
            return 9;
        case Lexer::EQUAL_TOKEN:
        case Lexer::NOT_EQUAL_TOKEN:
            return 8;
        case Lexer::MAND_TOKEN: return 7;
        case Lexer::MXOR_TOKEN: return 6;
        case Lexer::MOR_TOKEN: return 5;
        case Lexer::LAND_TOKEN: return 4;
        case Lexer::LOR_TOKEN:
        case Lexer::RANGE_TOKEN:
            return 3;
        case Lexer::SET_TOKEN:
        case Lexer::ADD_TO_TOKEN:
        case Lexer::SUB_TO_TOKEN:
        case Lexer::MUL_TO_TOKEN:
        case Lexer::DIV_TO_TOKEN:
        case Lexer::MOD_TO_TOKEN:
        case Lexer::SLEFT_TO_TOKEN:
        case Lexer::SRIGHT_TO_TOKEN:
        case Lexer::MOR_TO_TOKEN:
        case Lexer::MXOR_TO_TOKEN:
        case Lexer::MAND_TO_TOKEN:
        case Lexer::IN_TOKEN:
        case Lexer::OF_TOKEN:
            return 2;
//        case Lexer::COMMA_TOKEN: return 1;
        default: return 15;
    }
}