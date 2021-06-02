#include <lexer.h>
#include <parser.h>

#include <Type.h>
#include <Node/Scope.h>
#include <Node/Cast.h>

#include <iostream>

static Scope* global;


static char* readfile(const char* file) {
    FILE* f = fopen(file, "rb");
    if (!f) {
        return nullptr;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return nullptr;
    }

    long length = ftell(f);
    rewind(f);
    char* buf = (char*)malloc(length + 1);
    buf[length] = 0; // the buffer we pass to lex must be 0-terminated

    int nread = fread(buf, 1, length, f);
    if (nread < length) {
        fclose(f);
        free(buf);
        return nullptr;
    }

    fclose(f);
    return buf;
}




int main(int argc, const char** argv) {
    global = new Scope(nullptr);
    global->casts.push_back(new NumberLiteralToPrimitiveCast(&t_u16));

    char* f = readfile("test.kodo");
    if (!f) {
        std::cout << "Failed to read file \n";
        return 1;
    }

    if (!lex(f))
        return 1;

    if (!parse(global, tokens))
        return 1;

    if (!global->forward_declare_pass(nullptr))
        return 1;

    if (!global->resolve_pass(nullptr, nullptr, nullptr))
        return 1;


    std::cout << global << "\n\n\n";
    return 0;
}
