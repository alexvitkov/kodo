#include <Error.h>
#include <iostream>

#include <Type.h>
#include <Node/Scope.h>
#include <Node/Cast.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Variable.h>
#include <Node/IfStatement.h>

Type t_i8 (TOK_I8);
Type t_i16(TOK_I16);
Type t_i32(TOK_I32);
Type t_i64(TOK_I64);
Type t_u8 (TOK_U8);
Type t_u16(TOK_U16);
Type t_u32(TOK_U32);
Type t_u64(TOK_U64);
EtherealType t_number_literal(TOK_NUMBER_LITERAL);
EtherealType t_type(TOK_TYPE);

// VOLATILE - those must be ordered the same way as the TOK_* enum entries
Type* const primitive_numeric_types[8] = {
    &t_i8, &t_i16, &t_i32, &t_i64,
    &t_u8, &t_u16, &t_u32, &t_u64,
};

bool Type::is_ethereal()         { return false; } 
bool EtherealType::is_ethereal() { return true; } 

bool Node::resolve_children() {
    return true;
}

Node* Node::resolve(Scope* parent) {
    return resolve_children() ? this : nullptr;
}

bool Scope::resolve_children() {
    for (Node*& stmt : statements)
        MUST (stmt = stmt->resolve(this));
    return true;
}


static bool pick_non_template_overload(Call* call, Atom fn_atom, Scope* scope) {
    // FIXME this ignores shadowing
    std::vector<Function*> candidates;
    for (Scope* s = scope; s; s = s->parent) {
        for (auto& def : s->fn_definitions) {
            if (def.key == fn_atom)
                candidates.push_back(def.value);
        }
    }

    if (candidates.empty())
        return false;

    int min_friction = INT32_MAX;
    Function* best_overload = nullptr;
    std::vector<Node*> best_overload_args;

    std::vector<Node*> new_tmp_args(call->args.size());

    for (Function* overload : candidates) {
        i32 friction = 0;

        for (int i = 0; i < call->args.size(); i++) {
            if (!(new_tmp_args[i] = call->args[i]->cast(overload->get_fn_type()->params[i], scope, &friction)))
                goto NextOverload;
        }

        if (friction < min_friction) {
            min_friction  = friction;
            best_overload = overload;
            best_overload_args = new_tmp_args;
        }

NextOverload:;
    }

    if (!best_overload)
        return false;

    // delete (UnresolvedRef*)call->fn;
    call->fn = best_overload;
    call->args = best_overload_args;
    return true;
}

static bool pick_template_overload(Call* call, Atom fn_atom, Scope* scope) {
    std::vector<AST_Function*> candidates;
    for (Scope* s = scope; s; s = s->parent) {
        for (auto& def : s->templated_fn_definitions) {
            if (def.key == fn_atom)
                candidates.push_back(def.value);
        }
    }

    int min_friction = INT32_MAX;
    AST_Function* best_overload = nullptr;
    std::vector<Node*> best_overload_args;
    std::vector<Type*> best_overload_template_types;
    std::vector<Node*> new_tmp_args(call->args.size());

    for (AST_Function* ast_fn : candidates) {
        i32 friction = 0;
        std::vector<Type*> template_types(ast_fn->template_params.size());

        for (int i = 0; i < call->args.size(); i++) {
            Node* _param_type = ast_fn->params[i].type;
            Type* param_type = dynamic_cast<Type*>(_param_type); // FIXME type?
            UnresolvedRef* ph = dynamic_cast<UnresolvedRef*>(_param_type);

            if (param_type) {
                // non-templated parameter:
                if (!(new_tmp_args[i] = call->args[i]->cast(param_type, scope, &friction)))
                    goto NextOverload;
            } 

            else if (ph) {
                // templated parameter:

                // Find the index of the template parameter
                // in foo[A, B, C, D](...)    C is index 2
                int index = -1;
                for (int i = 0; i < ast_fn->template_params.size(); i++)
                    if (ph->atom == ast_fn->template_params[i]) {
                        index = i;
                        break;
                    }
                assert(index >= 0); // FIXME throw sane error here

                if (template_types[index]) {
                    // the template parameter has already been pinned to a type
                    if (!(new_tmp_args[i] = call->args[i]->cast(template_types[index], scope, &friction)))
                        goto NextOverload;
                } else {
                    // the template hasn't yet been resolved. pin it to the type of the argument
                    template_types[index] = call->args[i]->get_type();
                }
            }
            else {
                UNREACHABLE(); // we should've failed earlier when checking for invalid types
            }
        }

        if (friction < min_friction) {
            min_friction  = friction;
            best_overload = ast_fn;
            best_overload_args = new_tmp_args;
            best_overload_template_types = template_types;
        }

NextOverload:;
    }

    if (best_overload) {
        Function* new_fn = best_overload->get_instance(best_overload_template_types);
        MUST (new_fn);

        call->fn = new_fn;
        call->args = new_tmp_args;
        return true;
    }

    return false;
}



Node* Call::resolve(Scope* parent) {
    if (tried_resolved)
        return resolved ? this : nullptr;
    tried_resolved = true;

    Atom fn_atom = fn->as_atom_reference();

    if (fn_atom == ':') {
        // The call is actually a definition (x: int)
        Atom identifier = args[0]->as_atom_reference();
        if (!identifier) {
            add_error(new InvalidDeclarationError(this));
            return nullptr;
        }

        Type* type = as_runtime_type(args[1]->resolve(parent));
        MUST (type);

        return parent->define_variable(identifier, type, this);
    } 

    else if (fn_atom == TOK_INFERDECL) {
        // declaration without specified type, x := 123
        Atom identifier = args[0]->as_atom_reference();
        if (!identifier) {
            add_error(new InvalidDeclarationError(this));
            return nullptr;
        }

        Node* value = args[1]->resolve(parent);
        Type* variable_type = as_runtime_type(value->get_type());
        MUST (variable_type);

        if (variable_type != value->get_type())
            value = value->cast(variable_type, parent, nullptr);

        Variable* var = parent->define_variable(identifier, variable_type, this);

        Node* assignment = (new Call('=', var, value))->resolve(parent);
        return assignment;
    }

    else {
        // The call is an operator/regular fn call
        if (!fn_atom) {
            add_error(new InvalidCallError(this));
            return nullptr;
        }

        for (Node*& arg : args)
            MUST (arg = arg->resolve(parent));

        if (pick_non_template_overload(this, fn_atom, parent)) {
            resolved = true;
        } else if (pick_template_overload(this, fn_atom, parent)) {
            resolved = true;
        } else {
            add_error(new InvalidCallError(this));
            return nullptr;
        }

        return this;
    }
}



Node* UnresolvedRef::resolve(Scope* parent_scope) {
    for (Scope* s = parent_scope; s != nullptr; s = s->parent) {
        auto it = s->regular_namespace.find(atom);
        if (it != s->regular_namespace.end())
            return it->second;
    }
    return nullptr;
}

bool IfStatement::forward_declare_pass(Scope* scope) {
    MUST (root_scope->forward_declare_pass(scope));
    MUST (condition->forward_declare_pass(root_scope));
    MUST (then_block->forward_declare_pass(scope));
    if (else_block)
        MUST (else_block->forward_declare_pass(scope));
    return true;
}

bool IfStatement::resolve_children() {
    Node* _condition = condition->resolve(root_scope);
    MUST (_condition);
    condition = _condition;

    MUST (then_block->resolve_children());
    if (else_block)
        MUST (else_block->resolve_children());
    return true;
}

Type* Cast::get_type()          { NOT_IMPLEMENTED(); }
Type* IfStatement::get_type()   { NOT_IMPLEMENTED(); }
Type* Type::get_type()          { return &t_type; }
Type* Variable::get_type()      { return type; }
Type* UnresolvedRef::get_type() { return nullptr; }
Type* Scope::get_type()         { return nullptr; }

Type* Call::get_type() {
    if (resolved)
        return ((Function*)fn)->get_fn_type()->return_type;
    return nullptr;
}



