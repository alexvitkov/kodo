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

    if (!f->lex()) {
        std::cout << "lex() failed\n";
    } else if (!f->parse()) {
        std::cout << "parse() failed\n";
    } else if (!global->scope->forward_declare_pass(nullptr)) {
        std::cout << "forward_declare_pass() failed\n";
    } else if (!global->scope->resolve_children()) {
        std::cout << "resolve_pass() failed\n";
    } else {
        std::cout << global->scope << "\n\n\n";
        return 0;
    }

    std::cout << "\n";
    for (Error* err : global->errors) {
        err->print();
        std::cout << "\n\n";
    }

    return 0;
}
