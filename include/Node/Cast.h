#pragma once

#include <Node.h>
#include <Type.h>

struct Cast : Node {
    Type* source_type;
    Type* target_type;

    virtual void print(std::ostream& o, bool print_definition) override;
    virtual Type* get_type() override;
    virtual Node* get_node(Node* source) = 0;
};

struct NumberLiteralToPrimitiveCast : Cast {
    virtual Node* get_node(Node* source) override;
    NumberLiteralToPrimitiveCast(Type* destination_type);
};

struct SignedPrimitiveUpcast : Cast {
    virtual Node* get_node(Node* source) override;
    inline SignedPrimitiveUpcast(Type* target_type, Type* source_type) {
        this->target_type = target_type;
        this->source_type = source_type;
    }
};
