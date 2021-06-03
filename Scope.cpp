#include <Error.h>
#include <Node/Cast.h>
#include <Node/Scope.h>
#include <Node/Function.h>
#include <Node/Variable.h>


void Scope::init_global_scope() {
    for (Type* t : primitive_numeric_types) {
        casts.push_back(new NumberLiteralToPrimitiveCast(t));
        define_function('=', new DefaultAssignmentOperator(t));
    }
}

bool Scope::define_function(Atom key, Function* value) {
    for (const Definition& def : definitions) {
        if (def.key.atom == key.atom && def.value->type->equals(value->type, false)) {
            add_error(new AlreadyDefinedError({
                def.value,
                value
            }));
            return false;
        }
    }
    
    definitions.push_back({ key, value });
    return true;
}

Variable* Scope::define_variable(Atom key, Type* type, Node* source_node) {
    if (variables.contains(key)) {
        std::vector<Node*> defs {
            variables[key],
            source_node,
        };
        add_error(new AlreadyDefinedError(defs));
        return nullptr;
    }
    Variable* var = new Variable(key, this, type);
    variables[key] = var;
    return var;
}
