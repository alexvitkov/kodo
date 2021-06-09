#pragma once

#include <common.h>
#include <iosfwd>

struct Type;
struct Scope;
struct RuntimeValue;
struct Interpreter;


struct Node {
    virtual Type* get_type();
    virtual void  print(std::ostream& o, bool print_definition);
    virtual Node* clone(Scope* parent_scope);
    virtual RuntimeValue* evaluate(Interpreter* interpreter);

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

struct RuntimeValue : Node {
    Type* type;
    union {
        void* data;
        i64 int_value;
    };

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual RuntimeValue* evaluate(Interpreter* interpreter) override;
};


Type* as_type(Node* n);
Type* as_runtime_type(Node* n);
std::ostream& operator<<(std::ostream& o, Node* n);
