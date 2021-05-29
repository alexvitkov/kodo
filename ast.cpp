#include <ast.h>
#include <error.h>
#include <iostream>

std::ostream& operator<<(std::ostream& o, AST_Node* n) {
    n->print(o);
    return o;
}


thread_local int indent = 0;


bool AST_Function::add_argument(Atom identifier, AST_Node* type) {
    bool err = false;
    for (int i = 0; i < params.size(); i++)
        if (params[i].identifier == identifier) {
            err = true;
            add_error(new RepeatedArgumentError(this, i, params.size()));
        }

    params.push_back({ identifier, type });
    return !err;
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




void AST_Reference::print(std::ostream& o) {
    // TODO unresolved references could be colored red
    o << atom;
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

void AST_Block::define(Atom key, AST_Node* value) {
    definitions.push_back({ key, value });
}



void AST_Call::print(std::ostream& o) {

    AST_Reference* _fn = dynamic_cast<AST_Reference*>(fn);
    Atom atom = _fn->atom;

    if (atom.is_infix_operator()) {
        AST_Call* rhs_call = dynamic_cast<AST_Call*>(args[1]);

        o << args[0] << " " << fn << " ";

        if (rhs_call) {
            AST_Reference* rhs_fn = dynamic_cast<AST_Reference*>(rhs_call->fn);
            Atom rhs_atom = rhs_fn->atom;

            if (rhs_call && rhs_atom.is_infix_operator() && rhs_atom.precedence() < atom.precedence())
                o << "(" << args[1] << ")";
            return;
        }

        o << args[1];

    } else {
        o << fn << "(";
        for (int i = 0; i < args.size(); i++) {
            o << args[i];
            if (i != args.size() - 1)
                o << ", ";
        }
        o << ")";
    }
}
