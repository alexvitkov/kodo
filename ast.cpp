#include "type.h"
#include <ast.h>
#include <error.h>
#include <iostream>

thread_local static int indent = 0;


std::ostream& operator<<(std::ostream& o, Node* n) {
    n->print(o, false);
    return o;
}

bool Node::pass1(Scope* scope) { return true; }
bool Node::pass2(Node**, Scope* scope) { return true; }

Atom Node::as_atom_reference() {
    UnresolvedRef* ref;
    if ((ref = dynamic_cast<UnresolvedRef*>(this)))
        return ref->atom;
    return 0;
}


bool Function::pass1(Scope* scope) { 
    if (name)
        scope->define(name, this);

    MUST (body->pass1(scope));
    return true;
}


bool Function::pass2(Node** my_location, Scope* scope) { 
    // for (int i = 0; i < param_types.size(); i++) {
        // FIXME more strict checks on the form of the parameters
        // MUST (params[i]->pass2(&params[i], body));
    // }

    return body->pass2((Node**)&body, scope);
}

void Function::print(std::ostream& o, bool print_definition) {
    if (print_definition || !name) {
        o << "fn ";
        if (name)
            o << name;
        o << '(';
        for (int i = 0; i < type->params.size(); i++) {
            o << param_names[i] << ": " << type->params[i];
            if (i != type->params.size() - 1)
                o << ", ";
        }
        o << ") " << body;
    } else
        o << "\u001b[35m" << name << "\u001b[0m";
}



// bool AST_UnresolvedReference::pass2(AST_Node** my_location, AST_Block* scope) {
//     return true;
// }

void UnresolvedRef::print(std::ostream& o, bool print_definition) {
    o << "\u001b[31m" << atom << "\u001b[0m";
}


static void print_indent(std::ostream& o) {
    for (int i =0 ; i < indent; i++)
        o << "    ";
}


void Scope::print(std::ostream& o, bool print_definition) {
    o << "{\n";
    indent ++;
    for (auto& stmt : statements) {
        print_indent(o);
        stmt->print(o, true);
        o << ";\n";
    }
    indent --;
    print_indent(o);
    o << "}";
};

bool Scope::pass1(Scope* scope) {
    for (Node* n : statements)
        MUST (n->pass1(this));
    return true;
}

bool Scope::pass2(Node** my_location, Scope* scope) {
    for (int i = 0; i < statements.size(); i++)
        MUST (statements[i]->pass2(&statements[i], this));
    return true;
}

bool Scope::define(Atom key, Node* value) {
    definitions.push_back({ key, value });
    // std::cout << "Defined " << key << " := " << value << "\n\n\n";
    return true;
}

Variable* Scope::define_variable(Atom key, Type* type) {
    Variable* var = new Variable(key, this, type);
    variables[key] = var;
    return var;
}

void Variable::print(std::ostream& o, bool print_definition) {
    if (print_definition)
        o << name << ": " << type;
    else
        o << "\u001b[35m" << name << "\u001b[0m";
}

#define ALWAYS_BRACKETS


bool Call::pass2(Node** my_location, Scope* scope) { 
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

void Call::print(std::ostream& o, bool print_definition) {
    UnresolvedRef* _fn = dynamic_cast<UnresolvedRef*>(fn);
    if (_fn) {
        Atom atom = _fn->atom;

        if (atom.is_infix_operator()) {

#ifdef ALWAYS_BRACKETS
            o << '(' << args[0] << ' ' << fn << ' ' << args[1] << ')';
#else
            o << args[0] << " " << fn << " ";

            AST_Call* rhs_call = dynamic_cast<AST_Call*>(args[1]);
            if (rhs_call) {
                AST_Reference* rhs_fn = dynamic_cast<AST_Reference*>(rhs_call->fn);
                Atom rhs_atom = rhs_fn->atom;

                if (rhs_call && rhs_atom.is_infix_operator() && rhs_atom.precedence() < atom.precedence())
                    o << "(" << args[1] << ")";
                return;
            }
            o << args[1];
#endif

            return;
        } 
    }

    o << fn << "(";
    for (int i = 0; i < args.size(); i++) {
        o << args[i];
        if (i != args.size() - 1)
            o << ", ";
    }
    o << ")";
}

Node* Call::rotate() {
    UnresolvedRef* _fn = dynamic_cast<UnresolvedRef*>(fn);
    Atom fn_atom = _fn->atom;


    Call* rhs_call = dynamic_cast<Call*>(args[1]);
    if (!rhs_call) return this;

    auto _rhs_fn = dynamic_cast<UnresolvedRef*>(rhs_call->fn);
    if (!_rhs_fn) return this;
    Atom rhs_atom = _rhs_fn->atom;

    if (rhs_atom.precedence() < (fn_atom.precedence() + fn_atom.associativity()) && !rhs_call->brackets) {
        this->args[1] = rhs_call->args[0];
        rhs_call->args[0] = this;

        auto rotated_child = rhs_call->rotate();
        return rotated_child;
    }
    else {
        args[1] = rhs_call->rotate();
        return this;
    }
}





void Type::print(std::ostream& o, bool print_definition) {
    o << "\u001b[34m" << atom << "\u001b[0m";
}



void FunctionType::print(std::ostream& o, bool print_definition) {
    o << "(";
    for (Type* t : params)
        o << t << ", ";
    o << ") -> ";

    if (return_type)
        o << return_type;
    else 
        o << "void";
}
