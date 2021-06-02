#pragma once

#include <Node.h>
#include <vector>
#include <Cast.h>

struct NumberLiteral : Node {
    bool negative;
    std::vector<u8> digits;
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
};


struct CastedNumberLiteral : Node {
    i64 number;
    Type* type;

    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
};
