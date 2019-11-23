#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <tic.h>
#include "ast.h"

string Belish::AST::exportByString() {
    if (!root) return "null";
    return root->exportByString();
}
bool Belish::AST::Export(string filename) {
    std::ofstream file(filename, std::ios::app);
    if (!file) return false;
    file << exportByString();
    file.close();
    return true;
}
string Belish::AST::node::exportByString() {
    auto s = value();
    std::regex p1("\'(.)");
    std::regex p2("\"(.)");
    s = std::regex_replace(s, p1, "\\\'$1");
    s = std::regex_replace(s, p2, "\\\"$1");
    string res("node \"" + s + "\" line " + std::to_string(line()) + " children ");
    for (UL i = 0; i < children.size(); i++) {
        res += children[i]->exportByString();
    }
    res += " end ";
    return res;
}

void Belish::AST::importByString(string s) {
    byCache = true;
    astLexer.open(s);
}
void Belish::AST::import(string filename) {
    std::ifstream file(filename);
    string tmp;
    string s;
    while (getline(file, tmp)) {
        s += tmp + "\n";
    }
    file.close();
    importByString(s);
}
Belish::AST::~AST() {
    if (!child && root)
        delete root;
}

void Belish::AST::parse() {
    // 解析cache内容
    if (byCache) {
        if (root) delete root;
        root = nullptr;
#define AGET token = astLexer.get()
        auto AGET;
        if (token.t == Lexer::PROGRAM_END) {
            root = new node("PROGRAM-END", astLexer.l + baseLine);
            return;
        }
        if (token.s != "node") {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        AGET;
        string name(token.s);
        trim(name);
        UL findBase = 0;
        while (true) {
            auto i = name.find("\\", findBase);
            if (i == name.npos) break;
            name.erase(i, 1);
            while (i < name.length() && name[i] == '\\') i++;
            findBase = i;
        }
        AGET;
        if (token.s != "line") {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        AGET;
        if (token.t != Lexer::NUMBER_TOKEN) {
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        std::stringstream ss;
        UL line;
        ss << token.s;
        ss >> line;
        AGET;
        if (token.s != "children") {
            delete root;
            root = new node("bad-tree", astLexer.l + baseLine);
            root->insert("ASTCache Error: Unexpected token " + token.s, astLexer.l + baseLine);
            return;
        }
        UL indentCount = 1;
        AGET;
        string childrenContent;
        root = new node(name, line);
        while (true) {
            if (token.s == "end") {
                if (--indentCount < 1) break;
                if (indentCount == 1) {
                    childrenContent += " " + token.s;
                    auto ast = new AST("", astLexer.l + baseLine);
                    ast->importByString(childrenContent);
                    ast->parse();
                    CHECK(ast);
                    CHANGELINES(ast);
                    root->insert(ast->root);
                    delete ast;
                    childrenContent = "";
                    AGET;
                    continue;
                }
            }
            else if (token.s == "children") indentCount++;
            childrenContent += " " + token.s;
            AGET;
        }
        return;
    }

    // 解析script内容
    if (!child && root) delete root;
    root = nullptr;
    auto tmpIndex = lexer.i;
#define GET token = lexer.get()
    auto GET;
    if (token.t == Lexer::NOTE_TOKEN) GET;
    switch (token.t) {
    }
    if (!root) root = new node("pass", lexer.l + baseLine);
}

inline UL Belish::AST::priority(const string& op) {
    if (op == "[" || op == ".") return 15;
    if (op == "(") return 14;
    if (op == "++" || op == "--" || op == "!" || op == "~" || op == "**") return 13;
    if (op == "*" || op == "/" || op == "%") return 12;
    if (op == "+" || op == "-") return 11;
    if (op == "<<" || op == ">>") return 10;
    if (op == ">" || op == ">=" || op == "<" || op == "<=") return 9;
    if (op == "==" || op == "!=") return 8;
    if (op == "&") return 7;
    if (op == "^") return 6;
    if (op == "|") return 5;
    if (op == "&&") return 4;
    if (op == "||" || op == "~~") return 3;
    if (
            op == "="
            || op == "*=" || op == "/=" || op == "%="
            || op == "+=" || op == "-="
            || op == ">>=" || op == "<<="
            || op == "^=" || op == "&=" || op == "|="
            || op == "in" || op == "of"
            )
        return 2;
    if (op == ",") return 1;
    return 15;
}