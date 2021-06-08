#pragma once

#include <common.h>
#include <iosfwd>

struct Type;
struct Scope;

struct Node {
    virtual Type* get_type();
    virtual void print(std::ostream& o, bool print_definition);

    virtual bool forward_declare_pass(Scope* scope);

    virtual Node* resolve_pass(
        Type* wanted_type,
        int* friction,
        Scope* scope
    );

    Node* resolve_pass_cast_wrapper(
        Node** location,
        Type* wanted_type,
        int* friction,
        Scope* scope
    );

    // this method is used by the parser tests to check if
    // two nodes have the same structure
    // TODO this should only be in the test build
    virtual bool tree_compare(Node* other);
    
    virtual Node* clone();

    Atom as_atom_reference();

    virtual ~Node();
};

std::ostream& operator<<(std::ostream& o, Node* n);
