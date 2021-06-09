#include <Node.h>
#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>
#include <Node/IfStatement.h>

#include <iostream>

thread_local static int indent = 0;

static void print_indent(std::ostream& o) {

    for (int i =0 ; i < indent; i++)
        o << "    ";
}

std::ostream& operator<<(std::ostream& o, Node* n) {
    if (!n)
        o << "NULL"; 
    else
        n->print(o, false);
    return o;
}

void Function::print(std::ostream& o, bool print_definition) {
    o << name;
}

void AST_Function::print(std::ostream& o, bool print_definition) {
    if (print_definition || !name) {
        o << "fn ";
        if (name)
            o << name;

        if (!template_params.empty()) {
            o << '[';
            for (int i = 0; i < template_params.size(); i++) {
                o << template_params[i];
                if (i != template_params.size() - 1)
                    o << ", ";
            }

            o << ']';
        }


        o << '(';
        for (int i = 0; i < params.size(); i++) {
            o << params[i].name << ": " << params[i].type;
            if (i != params.size() - 1)
                o << ", ";
        }
        o << ") " << body;
    } else
        o << "\u001b[35m" << name << "\u001b[0m";
}

void UnresolvedRef::print(std::ostream& o, bool print_definition) {
    o << "\u001b[31m" << atom << "\u001b[0m";
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

void Variable::print(std::ostream& o, bool print_definition) {
    //if (print_definition)
        o << name << ": " << type;
    //else
        //o << "\u001b[35m" << name << "\u001b[0m";
}


#define ALWAYS_BRACKETS

void Call::print(std::ostream& o, bool print_definition) {
    UnresolvedRef* _fn = dynamic_cast<UnresolvedRef*>(fn);
    if (_fn) {
        Atom atom = _fn->atom;

        if (atom.is_infix_operator()) {

#ifdef ALWAYS_BRACKETS
            o << '(' << args[0] << ' ' << fn << ' ' << args[1] << ')';
#else
            o << args[0] << " " << fn << " ";

            Call* rhs_call = dynamic_cast<Call*>(args[1]);
            if (rhs_call) {
                Atom rhs_atom = rhs_call->fn->as_atom_reference();

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

void Cast::print(std::ostream& o, bool print_definition) {
    NOT_IMPLEMENTED();
}

void IfStatement::print(std::ostream& o, bool print_definition) {
    o << "if (" << condition << ") " << then_block;
    if (else_block)
        o << " else " << else_block;
}

void AST_Function_Instance::print(std::ostream& o, bool print_definition) {
    o << name;
    
    if (!template_types.empty()) {
        o << "[";
        for (int i = 0; i < template_types.size(); i++) {
            o << template_types[i];
            if (i != template_types.size() - 1)
                o << ", ";
        }
        o << "]";
    }

}
