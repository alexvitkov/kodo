#include <Node.h>
#include <Node/Scope.h>
#include <Node/Call.h>
#include <Node/UnresolvedRef.h>

bool Node::tree_compare(Node* other) {
    return false;
}

bool Scope::tree_compare(Node* _other) {
    Scope* other = dynamic_cast<Scope*>(_other);
    MUST (other);

    MUST (statements.size() == other->statements.size());

    for (int i = 0; i < statements.size(); i++)
        MUST (statements[i]->tree_compare(other->statements[i]));

    return true;
}

bool Call::tree_compare(Node* _other) {
    Call* other = dynamic_cast<Call*>(_other);
    MUST (other);
    
    MUST (fn->tree_compare(other->fn));
    MUST (args.size() == other->args.size());

    for (int i = 0; i < args.size(); i++)
        MUST (args[i]->tree_compare(other->args[i]));

    return true;
}

bool UnresolvedRef::tree_compare(Node* _other) {
    UnresolvedRef* other = dynamic_cast<UnresolvedRef*>(_other);
    MUST (other);

    return other->atom == atom;
}
