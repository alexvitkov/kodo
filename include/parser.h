#pragma once

#include <lexer.h>
#include <vector>

void init_keywords();
bool parse(const std::vector<Token>& tokens);
