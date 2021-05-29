#pragma once

#include <common.h>

#include <iosfwd>
#include <vector>

struct AST_Node {
    virtual void print(std::ostream& o) = 0;
};

struct AST_Function : AST_Node {
    struct Argument {
        Atom identifier;
        AST_Node* type;
    };

    AST_Node* body;
    std::vector<Argument> params;

    virtual void print(std::ostream& o) override;
    bool add_argument(Atom identifier, AST_Node* type);
};

struct AST_Reference : AST_Node {
    Atom atom;

    inline AST_Reference(Atom atom) : atom(atom) {}

    virtual void print(std::ostream& o) override;
};

struct AST_Block : AST_Node {
    virtual void print(std::ostream& o) override;
    std::vector<AST_Node*> statements;
};

std::ostream& operator<<(std::ostream& o, AST_Node* n);
