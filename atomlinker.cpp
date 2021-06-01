#include <atomlinker.h>

bool atomlink(Scope* scope) {
    if (!scope->pass1(nullptr))
        return false;
    if (!scope->pass2(nullptr, nullptr))
        return false;
    return true;
}
