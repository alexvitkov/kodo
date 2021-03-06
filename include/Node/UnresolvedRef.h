#pragma once

#include <Node.h>

struct UnresolvedRef : Node {
    Atom atom;

    inline UnresolvedRef(Atom atom) : atom(atom) {}

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual Node* resolve(Scope* parent) override;
    virtual Node* clone(Scope* parent_scope) override;

#ifdef _TEST
    virtual bool tree_compare(Node* other) override;
#endif
};
