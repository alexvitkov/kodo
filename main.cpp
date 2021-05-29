#include <lexer.h>
#include <parser.h>

char script[] = "fn haha (x: int, y: int) {}";

int main() {
    init_keywords();

    if (!lex(script))
        return 1;

    if (!parse(tokens))
        return 1;

    return 0;
}
