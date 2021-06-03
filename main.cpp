#include <GlobalContext.h>
#include <Type.h>
#include <Node/Scope.h>
#include <Node/Cast.h>

#include <iostream>




GlobalContext* global;



int main(int argc, const char** argv) {
    global = new GlobalContext();

    InputFile* f = global->add_source("test.kodo");
    if (!f)
        return 1;

    if (!f->lex())
        return 1;

    if (!f->parse())
        return 1;

    if (!global->scope->forward_declare_pass(nullptr))
        return 1;

    if (!global->scope->resolve_pass(nullptr, nullptr, nullptr))
        return 1;


    std::cout << global << "\n\n\n";
    return 0;
}
