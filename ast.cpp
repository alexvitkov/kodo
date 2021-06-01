#include <Type.h>
#include <ast.h>
#include <error.h>
#include <iostream>

thread_local static int indent = 0;


std::ostream& operator<<(std::ostream& o, Node* n) {
    n->print(o, false);
    return o;
}


Atom Node::as_atom_reference() {
    UnresolvedRef* ref;
    if ((ref = dynamic_cast<UnresolvedRef*>(this)))
        return ref->atom;
    return 0;
}





void Function::print(std::ostream& o, bool print_definition) {
    if (print_definition || !name) {
        o << "fn ";
        if (name)
            o << name;
        o << '(';
        for (int i = 0; i < type->params.size(); i++) {
            o << param_names[i] << ": " << type->params[i];
            if (i != type->params.size() - 1)
                o << ", ";
        }
        o << ") " << body;
    } else
        o << "\u001b[35m" << name << "\u001b[0m";
}



void UnresolvedRef::print(std::ostream& o, bool print_definition) {
    o << "\u001b[31m" << atom << "\u001b[0m";
}


static void print_indent(std::ostream& o) {
    for (int i =0 ; i < indent; i++)
        o << "    ";
}


void Scope::print(std::ostream& o, bool print_definition) {
    o << "{\n";
    indent ++;
    for (auto& stmt : statements) {
        print_indent(o);
        stmt->print(o, true);
        o << ";\n";
    }
    indent --;
    print_indent(o);
    o << "}";
};

bool Scope::define(Atom key, Node* value) {
    definitions.push_back({ key, value });
    // std::cout << "Defined " << key << " := " << value << "\n\n\n";
    return true;
}

Variable* Scope::define_variable(Atom key, Type* type) {
    Variable* var = new Variable(key, this, type);
    variables[key] = var;
    return var;
}

void Variable::print(std::ostream& o, bool print_definition) {
    if (print_definition)
        o << name << ": " << type;
    else
        o << "\u001b[35m" << name << "\u001b[0m";
}

#define ALWAYS_BRACKETS




void Call::print(std::ostream& o, bool print_definition) {
    UnresolvedRef* _fn = dynamic_cast<UnresolvedRef*>(fn);
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





void Type::print(std::ostream& o, bool print_definition) {
    o << "\u001b[34m" << atom << "\u001b[0m";
}



void FunctionType::print(std::ostream& o, bool print_definition) {
    o << "(";
    for (Type* t : params)
        o << t << ", ";
    o << ") -> ";

    if (return_type)
        o << return_type;
    else 
        o << "void";
}
