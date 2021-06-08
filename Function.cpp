#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <unordered_map>
#include <Node/Scope.h>

struct FunctionTypeHash {
    size_t operator() (const FunctionType& ft) const {
        size_t p = (size_t)ft.return_type;
        for (Node* n : ft.params) {
            p <<= 1;
            p ^= (size_t)n;
        }
        return p;
    }
};


bool FunctionType::equals(const FunctionType* other, bool compare_return_types) const {
    MUST (params == other->params);
    if (compare_return_types)
        MUST (return_type == other->return_type);
    return true;
}

std::unordered_map<FunctionType, FunctionType*, FunctionTypeHash> fn_types;

FunctionType* make_function_type_unique(FunctionType& temp) {
    auto it = fn_types.find(temp);
    if (it == fn_types.end()) {
        FunctionType* ft = new FunctionType(temp);
        fn_types[temp] = ft;
        return ft;
    } else {
        return it->second;
    }
}

FunctionType* FunctionType::get(Type* return_type, const std::vector<Type*>& param_types) {
    FunctionType temp(return_type, param_types);
    return make_function_type_unique(temp);
}


Type* Function::get_type() { 
    return get_fn_type(); 
}

Type* AST_Function::get_type() { 
    NOT_IMPLEMENTED(); 
}

FunctionType* AST_Function_OnlyInstance::get_fn_type() {
    return type;
}

FunctionType* AST_Function_Instance::get_fn_type() {
    return type;
}

FunctionType* DefaultAssignmentOperator::get_fn_type() { 
    return type; 
}

AST_Function_OnlyInstance::AST_Function_OnlyInstance(AST_Function* ast_fn) : ast_fn(ast_fn) {
    assert(ast_fn->template_params.size() == 0);

    FunctionType type_temp;
    type_temp.return_type = ast_fn->return_type;
    for (int i = 0; i < ast_fn->params.size(); i++)
        type_temp.params.push_back(ast_fn->params[i].type);

    type = make_function_type_unique(type_temp);

    body = new Scope(ast_fn->body->parent);
}

bool AST_Function::forward_declare_pass(Scope* scope) { 
    if (name) {
        if (template_params.empty()) {
            MUST (scope->define_function(name, new AST_Function_OnlyInstance(this)));
        } else {
            MUST (scope->define_function(name, this));
        }
    }

    MUST (body->forward_declare_pass(scope));
    return true;
}

Node* AST_Function_OnlyInstance::resolve_pass(Type* wanted_type, int* friction, Scope* scope) {
    // FIXME make sure argument types & template argument types are actually types
    for (int i = 0; i < type->params.size(); i++)
        body->define_variable(ast_fn->params[i].name, ast_fn->params[i].type, nullptr);

    FunctionType* _type = make_function_type_unique(*type);
    delete type;
    type = _type;

    MUST (body->resolve_pass(nullptr, nullptr, scope));
    return this;
    return this;
}

AST_Function_Instance::AST_Function_Instance(AST_Function* ast_fn, const std::vector<Type*>& template_args)
    : ast_fn(ast_fn) 
{
    name = ast_fn->name;
    body = (Scope*)ast_fn->body->clone();
    for (int i = 0;  i < template_args.size(); i++)
        body->redefine(ast_fn->template_params[i], template_args[i]);

    this->template_types = template_args;
}

Function* AST_Function::get_instance(const std::vector<Type*>& template_args) {
    // FIXME uniqueness
    Function* fn = new AST_Function_Instance(this, template_args);
    MUST (fn->forward_declare_pass(body->parent));
    MUST (fn->resolve_pass(nullptr, nullptr, body->parent));
    return fn;
}