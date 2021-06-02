#include <Node/Cast.h>
#include <Node/Scope.h>


void Scope::init_global_scope() {
    for (Type* t : primitive_numeric_types)
        casts.push_back(new NumberLiteralToPrimitiveCast(t));
}
