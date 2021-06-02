#pragma once

#include <Node.h>
#include <vector>
#include <unordered_map>

struct Definition {
    Atom key;
    Node* value;
};

struct Variable;
struct Cast;

struct Scope : Node {
    Scope* parent = nullptr;

    inline Scope(Scope* parent) : parent(parent) {}

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;

    std::vector<Node*> statements;
    std::vector<Definition> definitions;
    std::unordered_map<atom_t, Variable*> variables;
    std::vector<Cast*> casts;


    bool define(Atom key, Node* value);
    Variable* define_variable(Atom key, Type* type);

    virtual bool forward_declare_pass(Scope* scope) override;
    virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
};
