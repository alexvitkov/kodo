#pragma once

#include <common.h>

#include <iosfwd>
#include <vector>
#include <unordered_map>


struct Function;
struct UnresolvedRef;
struct Call;
struct Scope;
struct Variable;
struct Type;
struct FunctionType;

#include <Node.h>
#include <Type.h>
#include <NumberLiteral.h>

struct Function : Node {
    Atom name = 0;
    Scope* body;
    std::vector<Atom> param_names;
    FunctionType* type = nullptr;

    virtual Type* get_type() override;
    FunctionType* get_fn_type();
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual bool forward_declare_pass(Scope* scope) override;
    virtual bool resolve_pass(Node** my_location, Type* type, Scope* scope) override;
};

struct UnresolvedRef : Node {
    Atom atom;

    inline UnresolvedRef(Atom atom) : atom(atom) {}

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual int resolve_friction(Type* type, Scope* scope) override;
    virtual bool resolve_pass(Node** my_location, Type* type, Scope* scope) override;
};

struct Call : Node {
    bool brackets = false;
    bool resolved = false;
    bool tried_resolved = false;

    Node* fn = nullptr;
    std::vector<Node*> args;

    inline Call(Node* fn) : fn(fn) {}
    inline Call(Atom fn, Node* lhs, Node* rhs) : fn(new UnresolvedRef(fn)), args { lhs, rhs } {};

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual bool resolve_pass(Node** my_location, Type* type, Scope* scope) override;
    virtual int resolve_friction(Type* type, Scope* scope) override;
    Node* rotate();
};

struct Definition {
    Atom key;
    Node* value;
};

struct Scope : Node {
    Scope* parent = nullptr;

    inline Scope(Scope* parent) : parent(parent) {}

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;

    std::vector<Node*> statements;
    std::vector<Definition> definitions;
    std::unordered_map<atom_t, Variable*> variables;

    bool define(Atom key, Node* value);
    Variable* define_variable(Atom key, Type* type);

    virtual bool forward_declare_pass(Scope* scope) override;
    virtual bool resolve_pass(Node** my_location, Type* type, Scope* scope) override;
};

struct Variable : Node {
    Atom name;
    Scope* scope;
    Type* type;

    inline Variable(Atom name, Scope* block, Type* type) 
        : name(name), scope(block), type(type) {}
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
};




std::ostream& operator<<(std::ostream& o, Node* n);
