#include <atomlinker.h>

bool atomlink(Scope* scope) {
    if (!scope->forward_declare_pass(nullptr))
        return false;
    if (!scope->resolve_pass(nullptr, nullptr, nullptr))
        return false;
    return true;
}
