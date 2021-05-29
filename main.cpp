#include <lexer.h>
#include <parser.h>
#include <ast.h>

#include <iostream>

char script[] = "fn ++ -- ++--+-(x: int, y: int) { fn rofl() {}}";

static AST_Block* global;

int main() {
    global = new AST_Block();

    if (!lex(script))
        return 1;

    return 0;

    if (!parse(global, tokens))
        return 1;

    std::cout << global;

    return 0;
}
