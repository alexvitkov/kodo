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
    Node* rotate();

    virtual Type* get_type() override;
    virtual Node* clone(Scope* parent_scope) override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual Node* resolve(Scope* parent) override;
    virtual RuntimeValue* evaluate(Interpreter*) override;


#ifdef _TEST
    virtual bool tree_compare(Node* other) override;
#endif
};
