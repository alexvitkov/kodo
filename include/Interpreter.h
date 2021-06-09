#pragma once
#include <Node/Scope.h>
#include <vector>
#include <unordered_map>

struct Interpreter {
    
    struct ScopeWithData {
        Scope* scope;
        std::unordered_map<atom_t, RuntimeValue*> values;
    };

    std::vector<ScopeWithData> scopes;


    inline ScopeWithData& current_scope() { 
        return scopes.back();
    }

    inline void push_scope(Scope* s) {
        scopes.push_back({ s });
    }

    inline void pop_scope() {
        scopes.pop_back();
    }
};
