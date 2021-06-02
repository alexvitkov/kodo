#pragma once

#include <common.h>
#include <unordered_map>
#include <vector>


struct Context {
    bool closed;
    std::unordered_map<atom_t, struct AST_Node*> declared_nodes;
};
