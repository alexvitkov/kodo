#include <iostream>

#include <Error.h>
#include <Type.h>

#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>



Node::~Node() {}

Atom Node::as_atom_reference() {
    UnresolvedRef* ref;
    if ((ref = dynamic_cast<UnresolvedRef*>(this)))
        return ref->atom;
    return 0;
}




Node* Call::rotate() {
    UnresolvedRef* _fn = dynamic_cast<UnresolvedRef*>(fn);
    Atom fn_atom = _fn->atom;


    Call* rhs_call = dynamic_cast<Call*>(args[1]);
    if (!rhs_call) return this;

    auto _rhs_fn = dynamic_cast<UnresolvedRef*>(rhs_call->fn);
    if (!_rhs_fn) return this;
    Atom rhs_atom = _rhs_fn->atom;

    if (rhs_atom.precedence() < (fn_atom.precedence() + fn_atom.associativity()) && !rhs_call->brackets) {
        this->args[1] = rhs_call->args[0];
        rhs_call->args[0] = this;

        auto rotated_child = rhs_call->rotate();
        return rotated_child;
    }
    else {
        args[1] = rhs_call->rotate();
        return this;
    }
}




bool AST_Function::forward_declare_pass(Scope* scope) { 
    if (name)
        MUST (scope->define_function(name, this));

    MUST (body->forward_declare_pass(scope));
    return true;
}

bool Node::forward_declare_pass(Scope* scope) { 
    return true; 
}

bool Scope::forward_declare_pass(Scope* scope) {
    for (Node* n : statements)
        MUST (n->forward_declare_pass(this));
    return true;
}
