#pragma once

#include <vector>

#include <common.h>
#include <Error.h>
#include <Node/Scope.h>

struct InputFile {
    std::string path;
    char* buffer;
    std::vector<Token> tokens;

    bool lex();
    bool parse();
};

struct GlobalContext {
    Scope* scope;
    std::vector<Error*> errors;
    std::vector<struct InputFile*> input_files;

    inline GlobalContext() {
        scope = new Scope(nullptr);
    }

    InputFile* add_source(std::string path);
};

extern GlobalContext* global;
