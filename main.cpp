#include <lexer.h>
#include <parser.h>
#include <ast.h>

#include <iostream>

char script[] = "fn foo() { };";

static AST_Block* global;


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
    buf[length] = 0;

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
    global = new AST_Block();

    char* f = readfile("test.kodo");
    if (!f) {
        std::cout << "Failed to read file \n";
        return 1;
    }

    if (!lex(f))
        return 1;

    if (!parse(global, tokens))
        return 1;

    std::cout << global;

    return 0;
}
