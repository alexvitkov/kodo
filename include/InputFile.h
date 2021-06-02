#pragma once

#include <string>
#include <vector>

#include <Token.h>

struct InputFile {
    std::string path;
    char* buffer;
    std::vector<Token> tokens;
};

extern std::vector<InputFile*> input_files;

InputFile* load_file(std::string path);
