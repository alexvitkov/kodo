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
    o << "ref(" << atom << ")";
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
        o << stmt << "\n";
    }
    indent --;
    print_indent(o);
    o << "}\n";
};
