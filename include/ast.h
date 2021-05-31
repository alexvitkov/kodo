#pragma once

#include <common.h>

#include <iosfwd>
#include <vector>

struct AST_Node;
struct AST_Function;
struct AST_UnresolvedReference;
struct AST_Call;
struct AST_Block;

struct AST_Node {
    virtual void print(std::ostream& o) = 0;

    virtual bool define_tree(AST_Block* scope);

    Atom as_atom_reference();
};

struct AST_Function : AST_Node {
    struct Argument {
        Atom identifier;
        AST_Node* type;
    };

    Atom name = 0;
    AST_Block* body;
    std::vector<Argument> params;

    virtual void print(std::ostream& o) override;
    virtual bool define_tree(AST_Block* scope) override;
    bool add_argument(Atom identifier, AST_Node* type);
};

struct AST_UnresolvedReference : AST_Node {
    Atom atom;

    inline AST_UnresolvedReference(Atom atom) : atom(atom) {}

    virtual void print(std::ostream& o) override;
};

struct AST_Call : AST_Node {
    bool brackets = false;
    AST_Node* fn;
    std::vector<AST_Node*> args;


    inline AST_Call(AST_Node* fn) : fn(fn) {}
    inline AST_Call(Atom fn, AST_Node* lhs, AST_Node* rhs) : fn(new AST_UnresolvedReference(fn)), args { lhs, rhs } {};
    virtual void print(std::ostream& o) override;
    virtual bool define_tree(AST_Block* scope) override;
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

    bool define(Atom key, AST_Node* value);
    virtual bool define_tree(AST_Block* scope) override;
};

std::ostream& operator<<(std::ostream& o, AST_Node* n);
