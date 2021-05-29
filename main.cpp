#include <lexer.h>
#include <parser.h>
#include <ast.h>

#include <iostream>

char script[] = "fn foo(x:int,) {}";

static AST_Block* global;

int main() {
    global = new AST_Block();

    if (!lex(script))
        return 1;

    if (!parse(global, tokens))
        return 1;

    std::cout << global;

    return 0;
}
