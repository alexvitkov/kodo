#pragma once

#include <Token.h>
#include <Node.h>

#include <vector>

struct Type : Node {
    Atom atom;
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    inline Type(Atom atom) : atom(atom) {}
};

struct FunctionType : Type {
    Type* return_type;
    std::vector<Type*> params;
    
    inline FunctionType(): Type(0) {}

    bool equals(const FunctionType* other, bool compare_return_types = true) const;

    inline bool operator==(const FunctionType& other) const {
        return equals(&other, true);
    }

    inline FunctionType(Type* return_type, const std::vector<Type*>& param_types)
        : return_type(return_type), params(param_types), Type(0) {}

    static FunctionType* get(Type* return_type, const std::vector<Type*>& param_types);

    virtual void print(std::ostream& o, bool print_definition) override;
};

extern Type t_i8;
extern Type t_i16;
extern Type t_i32;
extern Type t_i64;
extern Type t_u8;
extern Type t_u16;
extern Type t_u32;
extern Type t_u64;
extern Type t_number_literal;
extern Type t_type;

extern Type* const primitive_numeric_types[8];
