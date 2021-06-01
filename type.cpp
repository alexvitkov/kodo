#include <ast.h>
#include <type.h>
#include <error.h>
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





bool Node::resolve_pass(Node**, Scope* scope) { 
    return true; 
}

bool Function::resolve_pass(Node** my_location, Scope* scope) { 
    // for (int i = 0; i < param_types.size(); i++) {
        // FIXME more strict checks on the form of the parameters
        // MUST (params[i]->resolve_pass(&params[i], body));
    // }

    return body->resolve_pass((Node**)&body, scope);
}

bool Scope::resolve_pass(Node** my_location, Scope* scope) {
    for (int i = 0; i < statements.size(); i++)
        MUST (statements[i]->resolve_pass(&statements[i], this));
    return true;
}

bool Call::resolve_pass(Node** my_location, Scope* scope) { 
    Atom fn_atom = fn->as_atom_reference();

    if (fn_atom == ':') {
        Atom identifier = args[0]->as_atom_reference();
        if (!identifier) {
            add_error(new InvalidDeclarationError(this));
            return false;
        }

        Type* type = dynamic_cast<Type*>(args[1]);
        if (!type) {
            add_error(new InvalidDeclarationError(this));
            return false;
        }

        Variable* var = scope->define_variable(identifier, type);
        MUST (var);
        *my_location = var;
        delete this;
    } else {
        if (!fn_atom) {
            add_error(new InvalidCallError(this));
            return false;
        }

        std::vector<Function*> possible_overloads;

        // FIXME this ignores shadowing
        for (Scope* s = scope; s; s = s->parent) {
            for (Definition& def : s->definitions) {
                if (def.key == fn_atom) {
                    Function* possible_fn = dynamic_cast<Function*>(def.value);

                    possible_overloads.push_back(possible_fn);
                }
            }
        }

        if (possible_overloads.empty()) {
            add_error(new InvalidCallError(this));
            return false;
        }

        int min_friction = INT32_MAX;
        Function* best_overload = nullptr;

        for (Function* overload : possible_overloads) {
            int friction = 0;
            for (int i = 0; i < args.size(); i++)
                friction += args[i]->resolve_friction(overload->get_fn_type()->params[i], scope);
            if (friction < min_friction) {
                min_friction = friction;
                best_overload = overload;
            }
        }

        if (!best_overload || min_friction >= INFINITE_FRICTION) {
            add_error(new InvalidCallError(this));
            return false;
        }

        delete (UnresolvedRef*)fn;
        fn = best_overload;

        for (int i = 0; i < args.size(); i++)
            args[i] = args[i]->resolve_as(best_overload->get_fn_type()->params[i], scope);
    }

    return true;
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
