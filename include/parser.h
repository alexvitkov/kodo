#pragma once

#include <lexer.h>
#include <vector>

bool parse(struct Scope* _global, const std::vector<Token>& tokens);
