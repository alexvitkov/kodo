#pragma once

#include <vector>
#include <Node/Cast.h>

struct NumberLiteral : Node {
    bool negative;
    std::vector<u8> digits;
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
    virtual RuntimeValue* evaluate(Interpreter* interpreter) override;
};
