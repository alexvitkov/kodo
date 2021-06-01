#include <ast.h>
#include <type.h>
#include <iostream>

Type t_type(0);
Type t_i8 (TOK_I8);
Type t_i16(TOK_I16);
Type t_i32(TOK_I32);
Type t_i64(TOK_I64);
Type t_u8 (TOK_U8);
Type t_u16(TOK_U16);
Type t_u32(TOK_U32);
Type t_u64(TOK_U64);




FunctionType* get(Type* return_type, const std::vector<Type*>& param_types) {
    // FIXME uniqueness
    return new FunctionType(return_type, param_types);
}




int Node::resolve_friction(Type* type, Scope* _) {
    return (type == get_type()) ? 0 : INFINITE_FRICTION;
}

int UnresolvedRef::resolve_friction(Type* type, Scope* scope) {
    return resolve_as(type, scope) ? 0 : INFINITE_FRICTION;
}



Node* Node::resolve_as(Type* type, Scope* _) {
    return (type == get_type()) ? this : nullptr;
}

Node* UnresolvedRef::resolve_as(Type* type, Scope* scope) {
    // assuming we're resolving a variable
    for (Scope* s = scope; s != nullptr; s = s->parent) {
        for (auto& vardecl: s->variables) {
            if (vardecl.first == atom && type == vardecl.second->type) 
                return vardecl.second;
        }
    }
    return nullptr;
}




Type* Type::get_type()          { return &t_type; }

Type* Function::get_type()      { return type; }
FunctionType* Function::get_fn_type() { return type; }
Type* Variable::get_type()      { return type; }

Type* UnresolvedRef::get_type() { return nullptr; }
Type* Call::get_type()          { return nullptr; }
Type* Scope::get_type()         { return nullptr; }
