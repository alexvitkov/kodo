#include <atomlinker.h>

bool atomlink(AST_Block* scope) {
    return scope->define_tree(nullptr);
}
