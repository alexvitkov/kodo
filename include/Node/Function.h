#pragma once

#include <Node.h>
#include <Type.h>
#include <vector>

struct Function : Node {
    Atom name = 0;
    virtual Type* get_type() override;
    virtual FunctionType* get_fn_type() = 0;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual RuntimeValue* evaluate(Interpreter*) override;
    virtual RuntimeValue* evaluate_call(Interpreter*, Slice<Node*> args) = 0;
};

struct Parameter {
    Atom name;
    Type* type;
};

struct AST_Function : Node {
    Atom name;
    struct Scope* body;
    Type* return_type;

    std::vector<Parameter> params;
    std::vector<Atom> template_params;

    Function* get_instance(const std::vector<Type*>& template_args);

    virtual void print(std::ostream& o, bool print_definition) override;
    virtual Type* get_type() override;
    virtual RuntimeValue* evaluate(Interpreter*) override;
    virtual bool forward_declare_pass(Scope* scope) override;
    // virtual Node* resolve_pass(Type* wanted_type, int* friction, Scope* scope) override;
};

struct AST_Function_Instance : Function {
    AST_Function* ast_fn;
    FunctionType* type;
    Scope* body;
    std::vector<Type*> template_types;

    AST_Function_Instance(AST_Function* ast_fn);
    AST_Function_Instance(AST_Function* ast_fn, const std::vector<Type*>& template_types);
    virtual bool resolve_children() override;
    virtual FunctionType* get_fn_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;

    virtual RuntimeValue* evaluate_call(Interpreter*, Slice<Node*> args) override;
};


struct DefaultAssignmentOperator : Function {
    FunctionType* type;
    Type* the_Type;

    virtual FunctionType* get_fn_type() override;
    virtual RuntimeValue* evaluate_call(Interpreter*, Slice<Node*> args) override;

    inline DefaultAssignmentOperator(Type* _type) : the_Type(_type) {
        name = '=';
        type = FunctionType::get(the_Type, { the_Type, the_Type });
    }
};


struct PrimitiveOperator : Function {
    FunctionType* fn_type;
    Type* operating_type;

    inline PrimitiveOperator(Atom atom, Type* operating_type) : operating_type(operating_type) {
        name = atom;
        fn_type = FunctionType::get(operating_type, { operating_type, operating_type });
    }

    virtual FunctionType* get_fn_type() override;
    virtual RuntimeValue* evaluate_call(Interpreter*, Slice<Node*> args) override;
};
