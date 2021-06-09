#include <iostream>

#include <Error.h>
#include <Type.h>

#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>

Node* Node::cast(Type* target_type, Scope* scope, i32* friction) {
    if (get_type() == target_type)
        return this;

    for (Scope* s = scope; s != nullptr; s = s->parent) {
        for (Cast* cast : s->casts) {
            if (cast->destination_type == target_type && cast->source_type == get_type()) {
                (*friction)++;
                return cast->get_node(this);
            }
        }
    }

    return nullptr;
}

Node* Node::clone() {
    NOT_IMPLEMENTED(); // child classes must override this
}

Type* Node::get_type() {
    NOT_IMPLEMENTED(); // child classes must override this
}

Type* as_type(Node* n) { 
    if (!n)
        return nullptr;

    Type* t = dynamic_cast<Type*>(n);

    if (!t)
        add_error(new NotATypeError(n));
    return t;
}

void Node::print(std::ostream& o, bool print_definition) {
    o << "NODE";
}

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





bool Node::forward_declare_pass(Scope* scope) { 
    return true; 
}

bool Scope::forward_declare_pass(Scope* scope) {
    for (Node* n : statements)
        MUST (n->forward_declare_pass(this));
    return true;
}
