#pragma once

#include <Node.h>
#include <Node.h>
#include <vector>
#include <unordered_map>

struct Variable;
struct Function;
struct AST_Function;
struct Cast;

template <typename T>
struct Definition {
    Atom key;
    T value;
};


struct Scope : Node {
    Scope* parent = nullptr;

    inline Scope(Scope* parent) : parent(parent) {
        if (!parent)
            init_global_scope();
    }

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;

    std::vector<Node*> statements;
    std::vector<Definition<Function*>> fn_definitions;
    std::vector<Definition<AST_Function*>> templated_fn_definitions;
    std::unordered_map<atom_t, Variable*> variables;
    std::vector<Cast*> casts;


    bool define_function(Atom key, AST_Function* value);
    bool define_function(Atom key, Function* value);

    Variable* define_variable(Atom key, Type* type, Node* source_node);

    virtual bool forward_declare_pass(Scope* scope) override;
    virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
    virtual bool tree_compare(Node* other) override;

private:
    void init_global_scope();
};
