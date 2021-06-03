#pragma once

#include <common.h>
#include <iosfwd>

struct Type;
struct Scope;

struct Node {
    virtual Type* get_type() = 0;
    virtual void print(std::ostream& o, bool print_definition) = 0;

    virtual bool forward_declare_pass(Scope* scope);

    virtual Node* resolve_pass(
        Type* wanted_type,
        int* friction,
        Scope* scope
    );

    Node* resolve_pass_cast_wrapper(
        Type* wanted_type,
        int* friction,
        Scope* scope
    );

    // this method is used by the parser tests to check if
    // two nodes have the same structure
    // TODO this should only be in the test build
    virtual bool tree_compare(Node* other);

    Atom as_atom_reference();

    virtual ~Node();
};

std::ostream& operator<<(std::ostream& o, Node* n);
