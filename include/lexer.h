#pragma once

#include <vector>

#include <Token.h>

extern bool lex(char* buffer);

extern std::vector<Token> tokens;
