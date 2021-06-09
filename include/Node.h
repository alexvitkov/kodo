#pragma once

#include <common.h>
#include <iosfwd>

struct Type;
struct Scope;

struct Node {
    virtual Type* get_type();
    virtual void  print(std::ostream& o, bool print_definition);
    virtual Node* clone();

    virtual bool  forward_declare_pass(Scope* scope);
    virtual bool  resolve_children();
    virtual Node* resolve(Scope* parent);

    virtual ~Node();


    Node* cast(Type* target_type, Scope* scope, i32* friction);
    Atom as_atom_reference();

    // this method is used by the parser tests to check if two nodes have the same structure
#ifdef _TEST
    virtual bool tree_compare(Node* other);
#endif

};

Type* as_type(Node* n);
std::ostream& operator<<(std::ostream& o, Node* n);
