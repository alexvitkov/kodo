#include <ast.h>

bool Node::forward_declare_pass(Scope* scope) { 
    return true; 
}

bool Function::forward_declare_pass(Scope* scope) { 
    if (name)
        scope->define(name, this);

    MUST (body->forward_declare_pass(scope));
    return true;
}

bool Scope::forward_declare_pass(Scope* scope) {
    for (Node* n : statements)
        MUST (n->forward_declare_pass(this));
    return true;
}
