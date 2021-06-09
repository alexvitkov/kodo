#include <Node.h>
#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>
#include <Node/IfStatement.h>

Node* Scope::clone(Scope* parent_scope) {
    Scope* new_scope = new Scope(*this);
    new_scope->parent = parent_scope;

    for (Node*& stmt : new_scope->statements)
        stmt = stmt->clone(new_scope);

    return new_scope;
}

Node* Call::clone(Scope* parent_scope) {
    Call* new_call = new Call(*this);

    for (Node*& arg : new_call->args)
        arg = arg->clone(parent_scope);

    return new Call(*this);
}

Node* Variable::clone(Scope* parent_scope) {
    Variable* var = new Variable(*this);
    var->scope = parent_scope;
    return var;

}

Node* UnresolvedRef::clone(Scope* parent_scope) {
    return new UnresolvedRef(atom);
}

Node* Type::clone(Scope*) {
    return this;
}
