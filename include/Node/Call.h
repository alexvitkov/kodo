#pragma once

#include <Node.h>
#include <Node/UnresolvedRef.h>
#include <vector>


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
    virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
    virtual bool tree_compare(Node* other) override;
    Node* rotate();
};
