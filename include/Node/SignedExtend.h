#pragma once

#include <Node.h>

struct SignedExtend : Node {
    Type* target_type;
    Node* source;

    inline SignedExtend(Type* target_type, Node* source) : target_type(target_type), source(source) {}
    virtual RuntimeValue* evaluate(Interpreter* interpreter) override;
};
