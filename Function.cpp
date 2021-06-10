#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Variable.h>
#include <iostream>
#include <unordered_map>
#include <Node/Scope.h>
#include <Interpreter.h>

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

FunctionType* AST_Function_Instance::get_fn_type() {
    return type;
}

FunctionType* DefaultAssignmentOperator::get_fn_type() { 
    return type; 
}

FunctionType* PrimitiveOperator::get_fn_type() { 
    return fn_type; 
}

bool AST_Function::forward_declare_pass(Scope* scope) { 
    if (name) {
        if (template_params.empty()) {
            MUST (scope->define_function(name, new AST_Function_Instance(this)));
        } else {
            MUST (scope->define_function(name, this));
        }
    }

    MUST (body->forward_declare_pass(scope));
    return true;
}

bool AST_Function_Instance::resolve_children() {

    for (Node* _param_type : type->params) {
        Type* param_type = as_type(_param_type->resolve(body));
        MUST (param_type);
    }


    for (int i = 0; i < type->params.size(); i++) {
        Type* t = as_type(type->params[i]->resolve(body));
        MUST (t);
        body->define_variable(ast_fn->params[i].name, t, nullptr);
    }

    MUST (body->resolve_children());
    return true;
}

AST_Function_Instance::AST_Function_Instance(AST_Function* ast_fn)
    : ast_fn(ast_fn) 
{
    name = ast_fn->name;
    body = (Scope*)ast_fn->body->clone(ast_fn->body->parent);

    FunctionType type_temp;
    type_temp.return_type = ast_fn->return_type;
    for (int i = 0; i < ast_fn->params.size(); i++) {
        // FIXME
        type_temp.params.push_back(ast_fn->params[i].type);
    }

    type = make_function_type_unique(type_temp);
}

AST_Function_Instance::AST_Function_Instance(AST_Function* ast_fn, const std::vector<Type*>& template_args)
    : ast_fn(ast_fn) 
{
    name = ast_fn->name;
    body = (Scope*)ast_fn->body->clone(ast_fn->body->parent);
    for (int i = 0;  i < template_args.size(); i++)
        body->define(ast_fn->template_params[i], template_args[i]);

    this->template_types = template_args;

    FunctionType type_temp;
    type_temp.return_type = ast_fn->return_type;
    for (int i = 0; i < ast_fn->params.size(); i++) {
        // FIXME
        type_temp.params.push_back(ast_fn->params[i].type);
    }
    type = make_function_type_unique(type_temp);

}

Function* AST_Function::get_instance(const std::vector<Type*>& template_args) {
    // FIXME uniqueness
    Function* fn = new AST_Function_Instance(this, template_args);
    MUST (fn->forward_declare_pass(body->parent));
    MUST (fn->resolve_children());
    return fn;
}

RuntimeValue* AST_Function::evaluate(Interpreter*) {
    return nullptr; // FIXME
}

RuntimeValue* Function::evaluate(Interpreter*) {
    return nullptr; // FIXME
}

RuntimeValue* AST_Function_Instance::evaluate_call(Interpreter* interpreter, Slice<Node*> args) {
    interpreter->push_scope(body);
    for (int i = 0; i < args.size; i++) {
        RuntimeValue* val = args[i]->evaluate(interpreter);
        MUST (val);
        interpreter->current_scope().values[ast_fn->params[i].name] = val;
    }

    auto val = body->evaluate(interpreter);

    interpreter->pop_scope();
    return val;
}

RuntimeValue* DefaultAssignmentOperator::evaluate_call(Interpreter* interpreter, Slice<Node*> args) {
    Variable* var = dynamic_cast<Variable*>(args[0]);
    assert(var); // FIXME
    RuntimeValue* val = args[1]->evaluate(interpreter);


    for (int i = interpreter->scopes.size() - 1; i >= 0; i--) {
        auto& s = interpreter->scopes[i];
        if (s.scope == var->scope) {
            s.values[var->name] = val;
            return val;
        }
    }

    return nullptr;
}

RuntimeValue* PrimitiveOperator::evaluate_call(Interpreter* interpreter, Slice<Node*> args) {
    RuntimeValue* r0 = args[0]->evaluate(interpreter);
    RuntimeValue* r1 = args[1]->evaluate(interpreter);
    MUST (r0 && r1);

    RuntimeValue* res = new RuntimeValue();
    res->type = this->operating_type;

    res->int_value = r0->int_value + r1->int_value;

    return res;
}
