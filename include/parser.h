#pragma once

#include <lexer.h>
#include <vector>

bool parse(struct AST_Block* _global, const std::vector<Token>& tokens);
