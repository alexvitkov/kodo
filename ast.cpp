#include <ast.h>
#include <error.h>
#include <iostream>

thread_local static int indent = 0;


std::ostream& operator<<(std::ostream& o, AST_Node* n) {
    n->print(o);
    return o;
}

bool AST_Node::define_tree(AST_Block* scope) { return true; }

Atom AST_Node::as_atom_reference() {
    AST_UnresolvedReference* ref;
    if ((ref = dynamic_cast<AST_UnresolvedReference*>(this)))
        return ref->atom;
    return 0;
}



bool AST_Function::add_argument(Atom identifier, AST_Node* type) {
    params.push_back({ identifier, type });
    return true;
}

bool AST_Function::define_tree(AST_Block* scope) { 
    if (name)
        MUST (scope->define(name, this));

    for (int i = 0; i < params.size(); i++)
        MUST (body->define(params[i].identifier, params[i].type));

    return body->define_tree(scope);
}

void AST_Function::print(std::ostream& o) {
    o << "fn (";
    for (int i = 0; i < params.size(); i++) {
        o << params[i].identifier << ": " << params[i].type;
        if (i != params.size() - 1)
            o << ", ";
    }
    o << ") " << body;
}




void AST_UnresolvedReference::print(std::ostream& o) {
    o << "\u001b[31m" << atom << "\u001b[0m";
}


static void print_indent(std::ostream& o) {
    for (int i =0 ; i < indent; i++)
        o << "    ";
}


void AST_Block::print(std::ostream& o) {
    o << "{\n";
    indent ++;
    for (auto& stmt : statements) {
        print_indent(o);
        o << stmt << ";\n";
    }
    indent --;
    print_indent(o);
    o << "}";
};


bool AST_Block::define_tree(AST_Block* scope) {
    for (AST_Node* n : statements)
        MUST (n->define_tree(this));
    return true;
}

bool AST_Block::define(Atom key, AST_Node* value) {
    definitions.push_back({ key, value });
    return true;
}

#define ALWAYS_BRACKETS


bool AST_Call::define_tree(AST_Block* scope) { 
    Atom fn_atom = this->as_atom_reference();
    if (fn_atom == ':') {
        Atom identifier = args[0]->as_atom_reference();
        if (!identifier) {
            add_error(new InvalidDeclarationError(this));
            return false;
        }

        MUST (scope->define(identifier, args[1]));
    }
    else {
        MUST (fn->define_tree(scope));

        for (AST_Node* n : args)
            MUST (n->define_tree(scope));
    }

    return true;
}

void AST_Call::print(std::ostream& o) {
    AST_UnresolvedReference* _fn = dynamic_cast<AST_UnresolvedReference*>(fn);
    if (_fn) {
        Atom atom = _fn->atom;

        if (atom.is_infix_operator()) {

#ifdef ALWAYS_BRACKETS
            o << '(' << args[0] << ' ' << fn << ' ' << args[1] << ')';
#else
            o << args[0] << " " << fn << " ";

            AST_Call* rhs_call = dynamic_cast<AST_Call*>(args[1]);
            if (rhs_call) {
                AST_Reference* rhs_fn = dynamic_cast<AST_Reference*>(rhs_call->fn);
                Atom rhs_atom = rhs_fn->atom;

                if (rhs_call && rhs_atom.is_infix_operator() && rhs_atom.precedence() < atom.precedence())
                    o << "(" << args[1] << ")";
                return;
            }
            o << args[1];
#endif

            return;
        } 
    }

    o << fn << "(";
    for (int i = 0; i < args.size(); i++) {
        o << args[i];
        if (i != args.size() - 1)
            o << ", ";
    }
    o << ")";
}

AST_Node* AST_Call::rotate() {
    AST_UnresolvedReference* _fn = dynamic_cast<AST_UnresolvedReference*>(fn);
    Atom fn_atom = _fn->atom;


    AST_Call* rhs_call = dynamic_cast<AST_Call*>(args[1]);
    if (!rhs_call) return this;

    auto _rhs_fn = dynamic_cast<AST_UnresolvedReference*>(rhs_call->fn);
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
