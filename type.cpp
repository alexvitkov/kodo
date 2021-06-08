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
Type t_number_literal(TOK_NUMBER_LITERAL);
Type t_type(TOK_TYPE);

// VOLATILE - those must be ordered the same way as the TOK_* enum entries
Type* const primitive_numeric_types[8] = {
    &t_i8, &t_i16, &t_i32, &t_i64,
    &t_u8, &t_u16, &t_u32, &t_u64,
};


Node* Node::resolve_pass_cast_wrapper(Node** location, Type* wanted_type, int* friction, Scope* scope) {
    Node* new_node = resolve_pass(wanted_type, friction, scope);
    if (new_node && location) {
        *location = new_node;
    }
    if (new_node && new_node->get_type() == wanted_type) {
        return new_node;
    }

    for (Scope* s = scope; s != nullptr; s = s->parent) {
        for (Cast* cast : s->casts) {
            if (cast->destination_type == wanted_type && cast->source_type == get_type())
                return cast->get_node(this);
        }
    }
    return nullptr;
}

Node* Node::resolve_pass(Type* wanted_type, int* friction, Scope* scope) {
    if (wanted_type && wanted_type != get_type())
        return nullptr;
    return this; 
}

Node* Scope::resolve_pass(Type*, int*, Scope* scope) {
    for (Node*& stmt : statements)
        MUST (stmt = stmt->resolve_pass(nullptr, nullptr, this));
    return this;
}

Node* Call::resolve_pass(Type* wanted_type, int*, Scope* scope) {
    if (tried_resolved) {
        if (!resolved || (wanted_type && wanted_type != get_type()))
            return nullptr;
        return this;
    }
    tried_resolved = true;

    Atom fn_atom = fn->as_atom_reference();

    if (fn_atom == ':') {
        Atom identifier = args[0]->as_atom_reference();
        if (!identifier) {
            add_error(new InvalidDeclarationError(this));
            return nullptr;
        }

        Type* type = (Type*)args[1]->resolve_pass(&t_type, 0, scope);
        if (!type) {
            add_error(new NotATypeError(args[1]));
            return nullptr;
        }

        return scope->define_variable(identifier, type, this);
    } else {
        if (!fn_atom) {
            add_error(new InvalidCallError(this));
            return nullptr;
        }

        std::vector<Function*> possible_overloads;

        // FIXME this ignores shadowing
        for (Scope* s = scope; s; s = s->parent) {
            for (auto& def : s->fn_definitions) {
                if (def.key == fn_atom) {
                    Function* possible_fn = dynamic_cast<Function*>(def.value);
                    possible_overloads.push_back(possible_fn);
                }
            }
        }

        if (possible_overloads.empty()) {
            add_error(new InvalidCallError(this));
            return nullptr;
        }

        int min_friction = INT32_MAX;

        Function* best_overload = nullptr;
        
        std::vector<Node*> best_overload_args;
        std::vector<Node*> new_tmp_args(args.size());

        for (Function* overload : possible_overloads) {
            i32 friction = 0;

            for (int i = 0; i < args.size(); i++) {
                Node* new_arg = args[i]->resolve_pass_cast_wrapper(&args[i], overload->get_fn_type()->params[i], &friction, scope);
                if (!new_arg)
                    goto NextOverload;
                new_tmp_args[i] = new_arg;
            }

            if (friction < min_friction) {
                min_friction  = friction;
                best_overload = overload;
                best_overload_args = new_tmp_args;
            }

NextOverload:;
        }

//         if (best_overload)
//             for (int i = 0; i < args.size(); i++)
//                 if (args[i] != best_overload_args[i])
//                     delete args[i];

        if (!best_overload) {
            add_error(new InvalidCallError(this));
            return nullptr;
        }

        delete (UnresolvedRef*)fn;
        fn = best_overload;
        args = best_overload_args;
    }

    resolved = true;

    return (!wanted_type || wanted_type == get_type()) ? this : nullptr;
}



Node* UnresolvedRef::resolve_pass(Type* type, int*, Scope* scope) {
    // assuming we're resolving a variable
    for (Scope* s = scope; s != nullptr; s = s->parent) {
        for (auto& vardecl: s->variables) {
            if (vardecl.first == atom && (!type || type == vardecl.second->type))
                return vardecl.second;
        }
    }
    return nullptr;
}

bool IfStatement::forward_declare_pass(Scope* scope) {
    MUST (root_scope->forward_declare_pass(scope));
    MUST (condition->forward_declare_pass(root_scope)); // is this needed?
    MUST (then_block->forward_declare_pass(scope));
    if (else_block)
        MUST (else_block->forward_declare_pass(scope));
    return true;
}

Node* IfStatement::resolve_pass(Type* type, int* friction, Scope* scope) {

    Node* _condition = condition->resolve_pass(nullptr, 0, root_scope);
    MUST (_condition);
    condition = _condition;

    MUST (then_block->resolve_pass(nullptr, nullptr, root_scope));
    if (else_block)
        MUST (else_block->resolve_pass(nullptr, nullptr, root_scope));
    return this;
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



