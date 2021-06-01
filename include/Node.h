#pragma once

#include <common.h>
#include <iosfwd>

struct Type;
struct Scope;

struct Node {
    virtual Type* get_type() = 0;
    virtual void print(std::ostream& o, bool print_definition) = 0;

    virtual bool forward_declare_pass(Scope* scope);

    virtual bool resolve_pass(Node** my_location, Type* type, Scope* scope);

    virtual int resolve_friction(Type* type, Scope* scope);

    Atom as_atom_reference();
};
