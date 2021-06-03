#pragma once

#include <Node.h>
#include <Type.h>
#include <vector>

struct Function : Node {
    Atom name = 0;
    FunctionType* type = nullptr;
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
};

struct AST_Function : Function {
    struct Scope* body;
    std::vector<Atom> param_names;

    virtual void print(std::ostream& o, bool print_definition) override;
    virtual bool forward_declare_pass(Scope* scope) override;
    virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
};

struct DefaultAssignmentOperator : Function {
    Type* the_Type;

    inline DefaultAssignmentOperator(Type* _type) : the_Type(_type) {
        name = '=';
        type = FunctionType::get(the_Type, { the_Type, the_Type });
    }
};
