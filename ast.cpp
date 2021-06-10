#include <iostream>

#include <Error.h>
#include <Type.h>
#include <Interpreter.h>

#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>

Node* Node::cast(Type* target_type, Scope* scope, i32* friction) {
    if (get_type() == target_type)
        return this;

    // FIXME
    // if we pass a Variable/Call of type number_literal for example
    // this will fail terribly as the cast expects a NumberLiteral
    // but we're giving it a Variable/Call
    for (Scope* s = scope; s != nullptr; s = s->parent) {
        for (Cast* cast : s->casts) {
            if (cast->destination_type == target_type && cast->source_type == get_type()) {
                if (friction) (*friction)++;
                return cast->get_node(this);
            }
        }
    }

    return nullptr;
}

// child classes must overrride those:
Node* Node::clone(Scope* parent_scope) { NOT_IMPLEMENTED(); } 
Type* Node::get_type() { NOT_IMPLEMENTED();  } 
RuntimeValue* Node::evaluate(Interpreter*) { NOT_IMPLEMENTED(); } 
bool Node::forward_declare_pass(Scope* scope) { return true; }


Type* RuntimeValue::get_type() {
    return type;
}

RuntimeValue* RuntimeValue::evaluate(Interpreter*) {
    return this;
}

bool RuntimeValue::operator==(const RuntimeValue& other) const {
    MUST (type == other.type);
    MUST (data == other.data);
    return true;
}



Type* as_type(Node* n) { 
    if (!n)
        return nullptr;

    Type* t = dynamic_cast<Type*>(n);

    if (!t)
        add_error(new NotATypeError(n));
    return t;
}

Type* as_runtime_type(Node* n) { 
    Type* t = as_type(n);
    // FIXME here we shall cast number_literal and other compile-time only types
    // to a default runtime type
    if (t == &t_number_literal)
        return &t_i64;
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
    else if (rhs_atom.is_infix_operator()) {
        args[1] = rhs_call->rotate();
    }
    return this;
}

RuntimeValue* Call::evaluate(Interpreter* interpreter) {
    Function* _fn = dynamic_cast<Function*>(fn);
    assert(_fn); 
    return _fn->evaluate_call(interpreter, { args.data(), args.size() });
}




RuntimeValue* Variable::evaluate(Interpreter* interpreter) {
    for (int i = interpreter->scopes.size() - 1; i >= 0; i--) {
        auto& s = interpreter->scopes[i];
        if (s.scope == scope)
            return s.values[name];
    }
    return nullptr;
}
