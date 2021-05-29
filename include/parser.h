#pragma once

#include <lexer.h>
#include <vector>

void init_keywords();
bool parse(struct AST_Block* _global, const std::vector<Token>& tokens);
