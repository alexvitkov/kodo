#pragma once

#include <Node.h>
#include <Node/Scope.h>

struct IfStatement : Node {
    Node* condition;
    Scope* root_scope;
    Scope* then_block;
    Scope* else_block = nullptr;

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;

    inline IfStatement(Scope* parent) {
        root_scope = new Scope(parent);
    }

    virtual bool forward_declare_pass(Scope* scope) override;
    virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
};
