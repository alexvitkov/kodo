#include <lexer.h>
#include <parser.h>

#include <InputFile.h>
#include <Type.h>
#include <Node/Scope.h>
#include <Node/Cast.h>

#include <iostream>

static Scope* global;






int main(int argc, const char** argv) {
    global = new Scope(nullptr);
    global->casts.push_back(new NumberLiteralToPrimitiveCast(&t_u16));

    InputFile* f = load_file("test.kodo");
    if (!f)
        return 1;

    if (!lex(f))
        return 1;

    if (!parse(global, f))
        return 1;

    if (!global->forward_declare_pass(nullptr))
        return 1;

    if (!global->resolve_pass(nullptr, nullptr, nullptr))
        return 1;


    std::cout << global << "\n\n\n";
    return 0;
}
