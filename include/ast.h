#ifndef BELISH_AST_H
#define BELISH_AST_H

#include <string>
#include "lex.h"
#include "values.h"
using std::string;
using std::vector;

namespace Belish {
    class ValueTracker;
    class AST {
    public:
        AST() : root(nullptr), script(""), child(false), baseLine(0), lexer(script) { }
        AST(const string& s) : root(nullptr), script(s), child(false), baseLine(0), lexer(script) { }
        void open(const string& s) {
            script = s;
            lexer.open(script);
        }
        void parse();
        void clear() {
            if (root) delete root;
            root = nullptr;
        }
        string value() { if (root) return root->value(); return ""; }
        ~AST();
    private:
        AST(const string& s, UL l) : root(nullptr), script(s), baseLine(l), child(true), lexer(script) { }
    public:
        class node {
            friend class ValueTracker;
        public:
            node() : v(""), l(0) {  }
            node(Lexer::TOKENS y, const string& s) : v(s), t(y), l(0) {  }
            node(Lexer::TOKENS y, const string& s, UL i) : v(s), t(y), l(i) {  }
            inline string value() const { return v; }
            inline void value(string c) { v = c; }
            inline Lexer::TOKENS type() const { return t; }
            inline void type(Lexer::TOKENS c) { t = c; }
            inline UL line() const { return l; }
            inline void line(UL t) { l = t; }
            inline node* get(long index) const {
                if (index < 0) index += children.size();
                if (children.empty()) return nullptr;
                return children[index % children.size()];
            }
            inline void set(long index, node* n) {
                if (children.empty()) return;
                while (index < 0) index += children.size();
                delete children[index % children.size()];
                children[index % children.size()] = n;
            }
            inline void noDelSet(long index, node* n) {
                if (children.empty()) return;
                while (index < 0) index += children.size();
                children[index % children.size()] = n;
            }
            inline UL length() const { return children.size(); }
            node& operator[](long index) { return *get(index); }
            void insert(node* n) { children.push_back(n); }
            void insert(Lexer::TOKENS y, const string& v, UL l) { children.push_back(new node(y, v, l)); }
            void clear() { children.clear(); }
            ~node() {
                LL sz = children.size();
                if (sz <= 0) return;
                if (nodevalue && !nodevalue->disbind()) delete nodevalue;
                for (auto i = 0; i < sz; i++) {
                    if (children[i]) delete children[i];
                }
            }
        private:
            Lexer::TOKENS t;
            UL l;
            Value* nodevalue = nullptr;
            vector<node*> children;
            string v;
        };
        bool child;
        UL baseLine;
        node* root;
        string script;
        Lexer lexer;
        bool isParsingClassMethod = false;
        bool isParsingClassCtorNew = false;
        friend class Compiler;
        static inline unsigned short priority(Lexer::TOKENS& tk);
    public:
        node* rValue() { return root; }
        UL line() {
            return root->line() + 1;
        }
    };
}

#endif //BELISH_AST_H
