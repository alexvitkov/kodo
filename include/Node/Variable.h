#pragma once

#include <Node.h>

struct Variable : Node {
    Atom name;
    Scope* scope;
    Type* type;

    inline Variable(Atom name, Scope* block, Type* type) 
        : name(name), scope(block), type(type) {}
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual Node* clone(Scope* parent_scope) override;
    virtual RuntimeValue* evaluate(Interpreter* i) override;
};
