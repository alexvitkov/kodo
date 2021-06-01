#pragma once

#include <Token.h>
#include <ast.h>

FunctionType* get(Type* return_type, const std::vector<Type*>& param_types);

extern Type t_i8;
extern Type t_i16;
extern Type t_i32;
extern Type t_i64;
extern Type t_u8;
extern Type t_u16;
extern Type t_u32;
extern Type t_u64;

const int INFINITE_FRICTION = 100000;
