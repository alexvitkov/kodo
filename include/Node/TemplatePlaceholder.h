#pragma once

#include <Node.h>


struct TemplatePlaceholder : Node {
    int index;
    Atom name;

    inline TemplatePlaceholder(int index, Atom name) : index(index), name(name) {}
    virtual Type* get_type() override;
    virtual void print(std::ostream& o, bool print_definition) override;
};
