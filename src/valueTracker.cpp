#include "valueTracker.h"

void Belish::ValueTracker::track(AST::node* node) {
    for (UL i = 0; i < node->length(); i++) track(node->get(i));
    switch (node->type()) {
        case Lexer::SET_TOKEN: {
            break;
        }
    }
}