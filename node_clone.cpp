#include <Node.h>
#include <Node/UnresolvedRef.h>
#include <Node/Function.h>
#include <Node/Call.h>
#include <Node/Scope.h>
#include <Node/Variable.h>
#include <Node/Cast.h>
#include <Node/IfStatement.h>
#include <Node/TemplatePlaceholder.h>

Node* Scope::clone() {
    Scope* new_scope = new Scope(*this);

    for (Node*& stmt : statements)
        stmt = stmt->clone();

    return new_scope;
}

Node* Call::clone() {
    return new Call(*this);
}
