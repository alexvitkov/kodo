#pragma once

#include <Node.h>
#include <Type.h>

struct Cast : Node {
    Type* source_type;
    Type* destination_type;

    virtual Node* get_node(Node* source) = 0;
};

struct NumberLiteralToPrimitiveCast : Cast {
    virtual Node* get_node(Node* source) override;
    NumberLiteralToPrimitiveCast(Type* source_type);
};
