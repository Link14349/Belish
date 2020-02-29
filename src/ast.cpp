#include <climits>
#include <list>
#include <cmath>
#include "ast.h"
#include "trans.h"
#include "trans.cpp"

#define FINISH_PRO token.t == Lexer::PROGRAM_END
#define FINISH_GET token.t == Lexer::PROGRAM_END || token.t == Lexer::END_TOKEN || token.t == Lexer::NO_STATUS
#define FINISH_CASE case Lexer::PROGRAM_END: case Lexer::END_TOKEN: case Lexer::NO_STATUS:
#define FINISH_GET_ERROR { delete root; \
root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token PROGRAM_END"); \
return; }

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
        case Lexer::NEW_TOKEN:
        {
            string exp;
#define AST_PARSE_GET_EXPR_PARSE_BC case Lexer::BRACKETS_LEFT_TOKEN: sbc++; break; \
            case Lexer::BRACKETS_RIGHT_TOKEN: sbc--; break; \
            case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN: mbc++; break; \
            case Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN: mbc--; break; \
            case Lexer::BIG_BRACKETS_LEFT_TOKEN: bbc++; break; \
            case Lexer::BIG_BRACKETS_RIGHT_TOKEN: bbc--; break;
            UL sbc = 0, mbc = 0, bbc = 0;
            string expr;
            auto defLine = lexer.line();
            while (true) {
                GET;
                switch (token.t) {
                    AST_PARSE_GET_EXPR_PARSE_BC
                    FINISH_CASE
                        if (!(sbc || mbc || bbc)) goto AST_PARSE_NEW_FINISH_GET;
                        break;
                }
                expr += token.s;
            }
            AST_PARSE_NEW_FINISH_GET:
            AST ast(expr, defLine + baseLine);
            ast.isParsingClassCtorNew = true;
            ast.parse();
            root = ast.root;
            root->type(Lexer::NEW_TOKEN);
            break;
        }
        case Lexer::CLASS_TOKEN:
        {
            auto defLine = lexer.line();
            GET;
            if (token.t != Lexer::UNKNOWN_TOKEN) {
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", defLine + baseLine);
                return;
            }
            root = new node(Lexer::CLASS_TOKEN, token.s, defLine + baseLine);
            root->insert(Lexer::NO_STATUS, "", defLine + baseLine);
            GET;
            if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) goto AST_PARSE_CLASS_METHODS_DEF;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", defLine + baseLine);
                return;
            }
            {
                UL bc = 1;
                string className;
                while (true) {
                    GET;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bc++;
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                        bc--;
                        if (!bc) goto AST_PARSE_PARSE_EX_CLASS_REF;
                    } else if (token.t == Lexer::COMMA_TOKEN && bc == 1) {
                        AST_PARSE_PARSE_EX_CLASS_REF:
                        if (className.empty()) break;
                        AST ast(className, lexer.line() + baseLine);
                        ast.parse();
                        AST_CHECK_PARSING_ERR(ast)
                        root->get(0)->insert(ast.root);
                        if (bc) continue;
                        break;
                    } else className += token.s + " ";
                }
                GET;
                if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", defLine + baseLine);
                    return;
                }
            }
            AST_PARSE_CLASS_METHODS_DEF:
            UL funDefLine = lexer.line();
            UL sbc = 0;
            UL bbc = 0;
            string funScript = "def ";
            while (true) {
                GET;
                funScript += token.s + " ";
                switch (token.t) {
                    case Lexer::BRACKETS_LEFT_TOKEN:
                        sbc++;
                        break;
                    case Lexer::BRACKETS_RIGHT_TOKEN:
                        sbc--;
                        break;
                    case Lexer::BIG_BRACKETS_LEFT_TOKEN:
                        bbc++;
                        break;
                    case Lexer::BIG_BRACKETS_RIGHT_TOKEN:
                        if (!bbc) goto AST_PARSE_FINISH_CLASS_PARSE;
                        bbc--;
                        if (!bbc) {
                            AST ast(funScript, funDefLine + baseLine);
                            ast.isParsingClassMethod = true;
                            ast.parse();
                            root->insert(ast.root);
                            funScript = "def ";
                            sbc = bbc = 0;
                        }
                        break;
                }
            }
            AST_PARSE_FINISH_CLASS_PARSE:
            break;
        }
        case Lexer::UNKNOWN_OP_TOKEN:
        {
            root = new node(Lexer::ERROR_TOKEN, "BLE103: Unknown operator '" + token.s + "'", lexer.line() + baseLine);
            break;
        }
        case Lexer::IMPORT_TOKEN:
        {
            GET;
            token.s.erase(0, 1);
            token.s.erase(token.s.length() - 1, 1);
            escape(token.s);
            root = new node(Lexer::IMPORT_TOKEN, token.s, lexer.line() + baseLine);
            LL index = token.s.find_last_of('/');
            if (index == std::string::npos) index = -1;
            string name;
            for (auto i = index + 1; i < token.s.length(); i++) name += token.s[i];
            root->insert(Lexer::NO_STATUS, name, lexer.line() + baseLine);
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
                UL sbc = 0, mbc = 0, bbc = 0;
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
        case Lexer::DEF_TOKEN:
        {
            auto defLine = lexer.line();
            GET;
            string name = token.s;
            GET;
            if (token.t != Lexer::BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            root = new node(Lexer::DEF_TOKEN, name, defLine + baseLine);
            root->insert(Lexer::NO_STATUS, "", defLine + baseLine);
            UL bc(1);
            UL argLine = lexer.line();
            string arg;
            if (isParsingClassMethod) {
                root->get(0)->insert(Lexer::UNKNOWN_TOKEN, "this", lexer.line() + baseLine);
            }
            while (true) {
                GET;
                if (FINISH_GET) FINISH_GET_ERROR
                if (token.t == Lexer::BRACKETS_LEFT_TOKEN) bc++;
                else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                    bc--;
                    if (!bc) goto ARG_DEF;
                } else if (token.t == Lexer::COMMA_TOKEN) {
                    if (bc == 1) {
                        ARG_DEF:
                        AST ast(arg, argLine + baseLine);
                        ast.parse();
                        root->get(0)->insert(ast.root);
                        arg = "";
                        argLine = lexer.line();
                        if (!bc) break;
                        continue;
                    }
                }
                arg += token.s + " ";
            }
            GET;
            if (token.t != Lexer::BIG_BRACKETS_LEFT_TOKEN) {
                delete root;
                root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'");
                return;
            }
            bc = 1;
            string body;
            UL bodyLine = lexer.line();
            while (true) {
                GET;
                if (FINISH_PRO) FINISH_GET_ERROR
                if (token.t == Lexer::BIG_BRACKETS_LEFT_TOKEN) bc++;
                else if (token.t == Lexer::BIG_BRACKETS_RIGHT_TOKEN) {
                    bc--;
                    if (!bc) break;
                }
                body += token.s + " ";
            }
            AST bodyParser(body, bodyLine + baseLine);
            while (true) {
                bodyParser.parse();
                if (bodyParser.root && bodyParser.root->type() != Lexer::PROGRAM_END) root->insert(bodyParser.root);
                else break;
            }
            break;
        }
        case Lexer::RETURN_TOKEN:
        {
            auto defLine = lexer.line();
            root = new node(Lexer::RETURN_TOKEN, "", defLine + baseLine);
            UL bbc = 0, mbc = 0, sbc = 0;
            string expr;
            while (true) {
                GET;
                switch (token.t) {
                    case Lexer::BIG_BRACKETS_LEFT_TOKEN: bbc++;break;
                    case Lexer::BIG_BRACKETS_RIGHT_TOKEN: bbc--;break;
                    case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN: mbc++;break;
                    case Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN: mbc--;break;
                    case Lexer::BRACKETS_LEFT_TOKEN: sbc++;break;
                    case Lexer::BRACKETS_RIGHT_TOKEN: sbc--;break;
                    case Lexer::END_TOKEN: if (!(bbc || mbc || sbc)) goto FINISH_RET_LOOP; break;
                }
                expr += token.s;
            }
            FINISH_RET_LOOP:
            AST ast(expr, defLine + baseLine);
            ast.parse();
            root->insert(ast.root);
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
                if (isParsingClassCtorNew) {
                    root->insert(Lexer::OBJECT_TOKEN, "", baseLine + initialLine);
                }
                bracketsCount = 1;
                while (true) {
                    GET;
                    if (FINISH_GET) break;
                    if (token.t == Lexer::BRACKETS_LEFT_TOKEN) {
                        bracketsCount++;
                        arg += token.s + " ";
                    }
                    else if (token.t == Lexer::BRACKETS_RIGHT_TOKEN) {
                        bracketsCount--;
                        if (!bracketsCount) {
                            if (arg.empty()) break;
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
                        if (argA.root->type() == Lexer::PROGRAM_END) {
                            arg = "";
                        }
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
        case Lexer::LNOT_TOKEN:
        {
            auto sl = lexer.line();
            root = new node(Lexer::LNOT_TOKEN, "", baseLine + lexer.line());
            string v;
            GET;
            while (!(FINISH_GET)) {
                v += token.s + " ";
                GET;
            }
            AST parser(v, baseLine + sl);
            parser.parse();
            root->insert(parser.root);
            break;
        }
        case Lexer::MNOT_TOKEN:
        {
            auto sl = lexer.line();
            root = new node(Lexer::MNOT_TOKEN, "", baseLine + lexer.line());
            string v;
            GET;
            while (!(FINISH_GET)) {
                v += token.s + " ";
                GET;
            }
            AST parser(v, baseLine + sl);
            parser.parse();
            root->insert(parser.root);
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
                        case Lexer::BIG_BRACKETS_LEFT_TOKEN: bbc++; val += token.s + " "; break;
                        case Lexer::BIG_BRACKETS_RIGHT_TOKEN: {
                            if (!bbc) goto FINISH_LOOP;
                            bbc--;
                            val += token.s + " ";
                            break;
                        }
                        case Lexer::MIDDLE_BRACKETS_LEFT_TOKEN: mbc++; val += token.s + " "; break;
                        case Lexer::MIDDLE_BRACKETS_RIGHT_TOKEN: mbc--; val += token.s + " "; break;
                        case Lexer::BRACKETS_LEFT_TOKEN: sbc++; val += token.s + " "; break;
                        case Lexer::BRACKETS_RIGHT_TOKEN: sbc--; val += token.s + " "; break;
                        case Lexer::COMMA_TOKEN: if (!(bbc || mbc || sbc)) goto FINISH_LOOP;
                        default: val += token.s + " ";
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
        case Lexer::DEBUGGER_TOKEN:
            root = new node(Lexer::DEBUGGER_TOKEN, "", lexer.line() + baseLine);
            break;
        case Lexer::GLOBAL_TOKEN: {
            root = new node(Lexer::GLOBAL_TOKEN, "", lexer.line() + baseLine);
            while (true) {
                auto preLine = lexer.line();
                GET;
                if (token.t != Lexer::UNKNOWN_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", preLine + baseLine);
                    return;
                }
                root->insert(Lexer::UNKNOWN_TOKEN, token.s, preLine + baseLine);
                GET;
                if (token.t == Lexer::END_TOKEN) break;
                if (token.t != Lexer::COMMA_TOKEN) {
                    delete root;
                    root = new node(Lexer::ERROR_TOKEN, "BLE104: Unexpected token '" + token.s + "'", preLine + baseLine);
                    return;
                }
            }
            break;
        }
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
        case Lexer::DOT_TOKEN:
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
        case Lexer::MEQ_TOKEN:
        case Lexer::LESS_TOKEN:
        case Lexer::LEQ_TOKEN:
            return 9;
        case Lexer::EQ_TOKEN:
        case Lexer::NEQ_TOKEN:
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

void Belish::AST::optimization() {
    if (root) root->optimization();
}

void Belish::AST::node::optimization() {
    /*
     * 优化语法树构建的结构
     * 比如将1+2的树形结构改为3
     * */
    for (UL i = 0; i < length(); i++) get(i)->optimization();
    if (type() >= Lexer::MAND_TOKEN && type() <= Lexer::SRIGHT_TOKEN && type() != Lexer::LNOT_TOKEN && type() != Lexer::MNOT_TOKEN) {
        auto left = get(0);
        auto right = get(1);
        auto opType = type();
        if (left->type() == Lexer::NUMBER_TOKEN && right->type() == Lexer::NUMBER_TOKEN) {
            type(Lexer::NUMBER_TOKEN);
            double lv(std::stod(left->value()));
            double rv(std::stod(right->value()));
            double val;
#define AST_NODE_OPT_EXPR_CASE(NAME, op) case Lexer::NAME##_TOKEN: val = lv op rv; break;
#define AST_NODE_OPT_EXPR_CASE_PI(NAME, op) case Lexer::NAME##_TOKEN: val = ((LL)lv) op ((LL)rv); break;
            switch (opType) {
                AST_NODE_OPT_EXPR_CASE(LAND, &&)
                AST_NODE_OPT_EXPR_CASE(LOR, ||)
                AST_NODE_OPT_EXPR_CASE(ADD, +)
                AST_NODE_OPT_EXPR_CASE(SUB, -)
                AST_NODE_OPT_EXPR_CASE(MUL, *)
                AST_NODE_OPT_EXPR_CASE(DIV, /)
                AST_NODE_OPT_EXPR_CASE_PI(MOD, %)
                AST_NODE_OPT_EXPR_CASE_PI(SLEFT, <<)
                AST_NODE_OPT_EXPR_CASE_PI(SRIGHT, >>)
                AST_NODE_OPT_EXPR_CASE_PI(MAND, &)
                AST_NODE_OPT_EXPR_CASE_PI(MOR, |)
                case Lexer::POWER_TOKEN:
                    val = std::pow(lv, rv);
                    break;
            }
            value(std::to_string(val));
        }
    }
}
