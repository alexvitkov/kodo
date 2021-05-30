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

struct AST_Call : AST_Node {
    bool brackets = false;
    AST_Node* fn;
    std::vector<AST_Node*> args;


    inline AST_Call(AST_Node* fn) : fn(fn) {}
    inline AST_Call(Atom fn, AST_Node* lhs, AST_Node* rhs) : fn(new AST_Reference(fn)), args { lhs, rhs } {};
    virtual void print(std::ostream& o) override;
    AST_Node* rotate();
};

struct Definition {
    Atom key;
    AST_Node* value;
};

struct AST_Block : AST_Node {
    virtual void print(std::ostream& o) override;

    std::vector<AST_Node*> statements;
    std::vector<Definition> definitions;

    void define(Atom key, AST_Node* value);
};

std::ostream& operator<<(std::ostream& o, AST_Node* n);
