#include <climits>
#include <vector>
#include <list>
#include <regex>
#include "ast.h"
#include "trans.h"
#include "trans.cpp"

#define FINISH_GET token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN || token.t == Lexer::NO_STATUS

Belish::AST::~AST() {
    if (!child && root)
        delete root;
}

#define AST_CHECK_PARSING_ERR(ast) if (ast.root->type() == Lexer::ERROR_TOKEN) { \
delete root; \
root = new node(Lexer::ERROR_TOKEN, ast.root->value(), ast.root->line()); \
return; \
}

void Belish::AST::parse() {
    if (!child && root) delete root;
    root = nullptr;
#define GET token = lexer.get()
    auto initialLine = lexer.line();
    auto initialIndex = lexer.index();
    auto GET;
    switch (token.t) {
        case Lexer::UNKNOWN_OP_TOKEN:
        {
            root = new node(Lexer::ERROR_TOKEN, "BLE103: Unknown operator '" + token.s + "'", lexer.line() + baseLine);
            break;
        }
        case Lexer::LET_TOKEN:
        {
            root = new node(Lexer::LET_TOKEN, "", initialLine);
            while (true) {
                GET;
                if (token.t == Lexer::END_TOKEN || token.t == Lexer::PROGRAM_END) break;
                root->insert(Lexer::UNKNOWN_TOKEN, token.s, lexer.line() + baseLine);
                GET;
                if (token.t == Lexer::COMMA_TOKEN) {
                    root->insert(Lexer::UNDEFINED_TOKEN, "", lexer.line() + baseLine);
                    continue;
                } else if (token.t != Lexer::SET_TOKEN && token.t != Lexer::PN_DREFER_TOKEN && token.t != Lexer::PN_IREFER_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE102: Incorrect assignment in declaration, unexpected token '" + token.s + "'", lexer.line() + baseLine);
                    return;
                }
                auto op = token;
                string value;
                ULL sbc = 0, mbc = 0, bbc = 0;
                auto defLine = lexer.line();
                while (true) {
                    GET;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) sbc++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) sbc--;
                    else if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bbc++;
                    else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) bbc--;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) mbc++;
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) mbc--;
                    else if ((token.t == Lexer::COMMA_TOKEN || FINISH_GET) && sbc == 0 && bbc == 0 && mbc == 0) break;
                    value += token.s + " ";
                }
                AST ast(value, baseLine + defLine);
                if (op.t == Lexer::PN_IREFER_TOKEN) ast.root = new node(Lexer::NO_STATUS, value, baseLine + defLine);
                else ast.parse();
                AST_CHECK_PARSING_ERR(ast)
                root->insert(op.t, "", baseLine + defLine);
                root->get(-1)->insert(ast.root);
                if (FINISH_GET) break;
            }
            break;
        }
        case Lexer::IF_TOKEN:
        {
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            root = new node(Lexer::IF_TOKEN, "", lexer.line() + baseLine);
            UL bcc = 1;
            auto conLine = lexer.line();
            string con;
            GET;
            while (true) {
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                con += token.s + " ";
                GET;
            }
            AST conAst(con, baseLine + conLine);
            conAst.parse();
            root->insert(conAst.root);
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            bcc = 1;
            string ifScript;
            auto ifSL = lexer.line();
            GET;
            while (true) {
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                ifScript += token.s + " ";
                GET;
            }
            root->insert(Lexer::NO_STATUS, "", ifSL + baseLine);
            AST bodyParser(ifScript, ifSL + baseLine);
            while (true) {
                bodyParser.parse();
                if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->get(1)->insert(bodyParser.root);
                else break;
            }
            while (true) {
                auto nowI = lexer.index();
                auto nowL = lexer.line();
                GET;
                if (token.t != Lexer::ELIF_TOKEN) {
                    lexer.index(nowI);
                    lexer.line(nowL);
                    break;
                }
                GET;
                if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                    return;
                }
                bcc = 1;
                conLine = lexer.line();
                con = "";
                GET;
                while (true) {
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcc++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                        bcc--;
                        if (bcc == 0) break;
                    }
                    con += token.s + " ";
                    GET;
                }
                conAst.open(con);
                conAst.baseLine = baseLine + conLine;
                conAst.parse();
                root->insert(conAst.root);
                GET;
                if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                    return;
                }
                bcc = 1;
                ifScript = "";
                ifSL = lexer.line();
                GET;
                while (true) {
                    if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                    else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                        bcc--;
                        if (bcc == 0) break;
                    }
                    ifScript += token.s + " ";
                    GET;
                }
                root->insert(Lexer::NO_STATUS, "", ifSL + baseLine);
                bodyParser.open(ifScript);
                bodyParser.baseLine = ifSL + baseLine;
                while (true) {
                    bodyParser.parse();
                    if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->get(-1)->insert(bodyParser.root);
                    else break;
                }
            }
            if (token.t == Lexer::ELSE_TOKEN) {
                ifScript = "";
                ifSL = lexer.line();
                GET;
                GET;
                if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                    return;
                }
                GET;
                bcc = 1;
                while (true) {
                    if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                    else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                        bcc--;
                        if (bcc == 0) break;
                    }
                    ifScript += token.s + " ";
                    GET;
                }
                root->insert(Lexer::NUMBER_TOKEN, "1", ifSL + baseLine);
                root->insert(Lexer::NO_STATUS, "", ifSL + baseLine);
                bodyParser.open(ifScript);
                bodyParser.baseLine = ifSL + baseLine;
                while (true) {
                    bodyParser.parse();
                    if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->get(-1)->insert(bodyParser.root);
                    else break;
                }
            }
            break;
        }
        case Lexer::NUMBER_TOKEN:
        case Lexer::STRING_TOKEN:
        case Lexer::BRACKETS_LEFT_TOKEN:
        case Lexer::UNKNOWN_TOKEN:
        case Lexer::UNDEFINED_TOKEN:
        case Lexer::NULL_TOKEN:
        {
            auto tmp_token = token;
            GET;
            if (FINISH_GET) {
                if (tmp_token.t == Lexer::STRING_TOKEN) {
                    tmp_token.s.erase(0, 1);
                    tmp_token.s.erase(tmp_token.s.length() - 1, 1);
                    escape(tmp_token.s);
                }
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
            bool hasOp = false;
            bool preTokenIsUnknown = tmp_token.t == Lexer::UNKNOWN_TOKEN;// included left brackets
            lexer.index(initialIndex);
            lexer.line(initialLine);
            ULL base = 1;
            GET;
            while (true) {
                if (FINISH_GET) break;
                if (token.t > Lexer::NOTE_TOKEN) {
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN || token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        base *= 14;
                        if (preTokenIsUnknown) {// 说明是函数/取值符
                            if (base <= (ULL)op.priority) {
                                hasOp = true;
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
                    } else {
                        preTokenIsUnknown = false;
                        if (base * (ULL)priority(token.t) <= (ULL)op.priority) {
                            hasOp = true;
                            op.token = token;
                            op.line = lexer.line();
                            op.index = lexer.index();
                            op.priority = base * (ULL)priority(token.t);
                        }
                    }
                } else preTokenIsUnknown = true;
                GET;
            }
            if (!hasOp) {
                root = new node(Lexer::ERROR_TOKEN, "BLE101: Wrong expression", baseLine + lexer.line());
                return;
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
            if (op.token.t == Lexer::UNKNOWN_OP_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE103: Unknown operator '" + op.token.s + "'", op.line + baseLine);
                return;
            }
            root = new node(op.token.t, "", op.line + baseLine);
            if (root->type() == Lexer::BRACKETS_LEFT_TOKEN) {
                AST la(left, baseLine + initialLine);
                la.parse();
                AST_CHECK_PARSING_ERR(la)
                root->insert(la.root);

                lexer.index(op.index);
                lexer.line(op.line);
                string arg;
                bracketsCount = 1;
                while (true) {
                    GET;
                    if (FINISH_GET) {
                        break;
                    }
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                        bracketsCount++;
                        arg += token.s + " ";
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                        bracketsCount--;
                        if (!bracketsCount) {
                            AST argA(arg, baseLine + initialLine);
                            argA.parse();
                            AST_CHECK_PARSING_ERR(argA)
                            root->insert(argA.root);
                            break;
                        }
                        arg += token.s + " ";
                    }
                    else if (token.t == Lexer::COMMA_TOKEN && bracketsCount == 1) {
                        AST argA(arg, baseLine + initialLine);
                        argA.parse();
                        AST_CHECK_PARSING_ERR(argA)
                        root->insert(argA.root);
                        arg = "";
                    } else arg += token.s + " ";
                }
            } else if (root->type() == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                AST la(left, baseLine + initialLine);
                la.parse();
                AST_CHECK_PARSING_ERR(la)
                root->insert(la.root);

                lexer.index(op.index);
                lexer.line(op.line);
                string attr;
                bracketsCount = 1;
                while (true) {
                    GET;
                    if (FINISH_GET) {
                        break;
                    }
                    if (token.t == Lexer::MIDDLE_BRACKETS_LEFT_TOKEN) {
                        bracketsCount++;
                        attr += token.s + " ";
                    }
                    else if (token.t == Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN) {
                        bracketsCount--;
                        if (!bracketsCount) {
                            AST attrA(attr, baseLine + initialLine);
                            attrA.parse();
                            AST_CHECK_PARSING_ERR(attrA)
                            root->insert(attrA.root);
                            break;
                        }
                        attr += token.s + " ";
                    } else attr += token.s + " ";
                }
                __asm("nop");
            } else {
                bracketsCount = 0;
                string right;
                lexer.index(op.index);
                lexer.line(op.line);
                GET;
                while (token.t != Lexer::END_TOKEN && token.t != Lexer::PROGRAM_END) {
                    right += token.s + " ";
                    if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) bracketsCount++;
                    else if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bracketsCount--;
                    GET;
                }
                while (bracketsCount--) {
                    auto tmp = right.rfind(')');
                    if (tmp == 0) right.erase(0, tmp + 1);
                    else right.erase(tmp - 1, tmp + 1);
                }
                AST la(left, baseLine + initialLine);
                AST ra(right, baseLine + initialLine);
                la.parse();
                if (!la.root || la.root->type() == Lexer::PROGRAM_END || la.root->type() == Lexer::END_TOKEN) {
                    // 说明该运算符是单目运算符且是在操作数左边比如：++a
                    string expr(op.token.s + right);
                    AST parser(expr, baseLine + initialLine);
                    parser.parse();
                    delete root;
                    root = parser.root;
                    lexer.line(lexerLine);
                    break;
                }
                AST_CHECK_PARSING_ERR(la)
                ra.parse();
                if (!ra.root || ra.root->type() == Lexer::PROGRAM_END || ra.root->type() == Lexer::END_TOKEN) {
                    // 说明该运算符是单目运算符且是在操作数右边比如：a++
                    root->insert(la.root);
                    lexer.line(lexerLine);
                    break;
                }
                AST_CHECK_PARSING_ERR(ra)
                root->insert(la.root);
                root->insert(ra.root);
            }
            lexer.line(lexerLine);
            break;
        }
        case Lexer::DADD_TOKEN:
            root = new node(Lexer::ADD_TO_TOKEN, "", baseLine + lexer.line());
        case Lexer::DSUB_TOKEN:
        {
            auto sl = lexer.line();
            if (!root) root = new node(Lexer::SUB_TO_TOKEN, "", baseLine + lexer.line());
            string v;
            GET;
            while (!(FINISH_GET)) {
                v += token.s + " ";
                GET;
            }
            AST parser(v, baseLine + sl);
            parser.parse();
            root->insert(parser.root);
            root->insert(Lexer::NUMBER_TOKEN, "1", baseLine + sl);
            break;
        }
        case Lexer::WHILE_TOKEN:
        {
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            root = new node(Lexer::WHILE_TOKEN, "", lexer.line() + baseLine);
            UL bcc = 1;
            auto conLine = lexer.line();
            string con;
            GET;
            while (true) {
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                con += token.s + " ";
                GET;
            }
            AST conAst(con, baseLine + conLine);
            conAst.parse();
            root->insert(conAst.root);
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            bcc = 1;
            string whileScript;
            auto whileSL = lexer.line();
            GET;
            while (true) {
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                whileScript += token.s + " ";
                GET;
            }
            AST bodyParser(whileScript, whileSL + baseLine);
            while (true) {
                bodyParser.parse();
                if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->insert(bodyParser.root);
                else break;
            }
            break;
        }
        case Lexer::FOR_TOKEN:
        {
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            root = new node(Lexer::FOR_TOKEN, "", lexer.line() + baseLine);
            UL bcc = 1;
            auto conLine = lexer.line();
            string con;
            GET;
            while (true) {
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                con += token.s + " ";
                GET;
            }
            root->insert(Lexer::NO_STATUS, "", lexer.line() + baseLine);
            AST conAst(con, baseLine + conLine);
            while (true) {
                conAst.parse();
                if (conAst.root->type() == Lexer::PROGRAM_END) break;
                root->get(0)->insert(conAst.root);
            }
            bcc = 1;
            string forScript;
            auto forSL = lexer.line();
            GET;
            GET;
            while (true) {
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                forScript += token.s + " ";
                GET;
            }
            AST bodyParser(forScript, forSL + baseLine);
            while (true) {
                bodyParser.parse();
                if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->insert(bodyParser.root);
                else break;
            }
            break;
        }
        case Lexer::DO_TOKEN:
        {
            root = new node(Lexer::DO_TOKEN, "", lexer.line() + baseLine);
            root->insert(nullptr);// 先占位(给条件用)
            UL bcc = 1;
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            string whileScript;
            auto whileSL = lexer.line();
            GET;
            while (true) {
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                whileScript += token.s + " ";
                GET;
            }
            AST bodyParser(whileScript, whileSL + baseLine);
            while (true) {
                bodyParser.parse();
                if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->insert(bodyParser.root);
                else break;
            }
            GET;
            if (token.t != Lexer::WHILE_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }bcc = 1;
            auto conLine = lexer.line();
            string con;
            GET;
            while (true) {
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bcc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bcc--;
                    if (bcc == 0) break;
                }
                con += token.s + " ";
                GET;
            }
            AST conAst(con, baseLine + conLine);
            conAst.parse();
            root->set(0, conAst.root);
            break;
        }
        case Lexer::BIG_BRACKETS_LEFT_TOKEN:
        {
            root = new node(Lexer::OBJECT_TOKEN, "", lexer.line() + baseLine);
            while (true) {
                GET;
                if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) break;
                if (token.t != Lexer::STRING_TOKEN && token.t != Lexer::UNKNOWN_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", lexer.line() + baseLine);
                    break;
                }
                if (token.t == Lexer::STRING_TOKEN) {
                    token.s.erase(0, 1);
                    token.s.erase(token.s.length() - 1, 1);
                    escape(token.s);
                }
                root->insert(Lexer::STRING_TOKEN, token.s, lexer.line() + baseLine);
                GET;
                if (token.t != Lexer::COLON_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", lexer.line() + baseLine);
                    break;
                }
                UL bbc = 0, mbc = 0, sbc = 0;
                string val;
                auto valLine = lexer.line();
                while (true) {
                    GET;
                    switch (token.t) {
                        case Lexer::BIG_BRACKETS_LEFT_TOKEN: bbc++; val += token.s; break;
                        case Lexer::BIG_BRACKETS_RIGHT_TOKEN: {
                            if (!bbc) goto FINISH_LOOP;
                            bbc--;
                            val += token.s;
                            break;
                        }
                        case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN: mbc++; val += token.s; break;
                        case Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN: mbc--; val += token.s; break;
                        case Lexer::BRACKETS_LEFT_TOKEN: sbc++; val += token.s; break;
                        case Lexer::BRACKETS_RIGHT_TOKEN: sbc--; val += token.s; break;
                        case Lexer::COMMA_TOKEN: if (!(bbc || mbc || sbc)) goto FINISH_LOOP;
                        default: val += token.s;
                    }
                }
                FINISH_LOOP:
                AST parser(val, valLine + baseLine);
                parser.parse();
                root->insert(parser.root);
                if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) break;
            }
            break;
        }
        case Lexer::END_TOKEN:
            root = new node(Lexer::END_TOKEN, "", lexer.line() + baseLine);
            break;
        case Lexer::BREAK_TOKEN:
            root = new node(Lexer::BREAK_TOKEN, "", lexer.line() + baseLine);
            break;
        case Lexer::CONTINUE_TOKEN:
            root = new node(Lexer::CONTINUE_TOKEN, "", lexer.line() + baseLine);
            break;
        case Lexer::PROGRAM_END:
            root = new node(Lexer::PROGRAM_END, "", lexer.line() + baseLine);
            break;
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