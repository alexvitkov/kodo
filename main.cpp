#include <GlobalContext.h>
#include <Type.h>
#include <Node/Scope.h>
#include <Node/Cast.h>
#include <Interpreter.h>

#include <iostream>




GlobalContext* global;



int main(int argc, const char** argv) {
    global = new GlobalContext();

    InputFile* f;
    if (argc == 2) {
        f = global->add_source(argv[1]);
    } else {
        f = global->add_source("test.kodo");
    }

    if (!f) {
        std::cout << "add_source() failed\n";
    } else if (!f->lex()) {
        std::cout << "lex() failed\n";
    } else if (!f->parse()) {
        std::cout << "parse() failed\n";
    } else if (!global->scope->forward_declare_pass(nullptr)) {
        std::cout << "forward_declare_pass() failed\n";
    } else if (!global->scope->resolve_children()) {
        std::cout << "resolve_pass() failed\n";
    } else {
        std::cout << global->scope << "\n\n\n";
        Interpreter* interpreter = new Interpreter();
        interpreter->push_scope(global->scope);
        std::cout << "Result: " << global->scope->evaluate(interpreter) << "\n";
        return 0;
    }


    std::cout << "\n";
    for (Error* err : global->errors) {
        err->print();
        std::cout << "\n\n";
    }

    return 0;
}
