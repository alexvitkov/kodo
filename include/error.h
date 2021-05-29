#pragma once

#include <iostream>
#include <Token.h>


#define ERROR(msg) { std::cerr << msg; return 0; }

struct Error {
    virtual void print() = 0;
};

struct UnexpectedTokenError : Error {
    Token actual;
    Atom expected;

    inline UnexpectedTokenError(Token actual, Atom expected) : actual(actual), expected(expected) {};
    virtual void print() override;
};

struct RepeatedArgumentError : Error {
    struct AST_Function* fn;
    int arg1_index, arg2_index;

    inline RepeatedArgumentError(AST_Function* fn, int arg1_index, int arg2_index) 
        : fn(fn), arg1_index(arg1_index), arg2_index(arg2_index) {}

    virtual void print() override;
};

enum ErrorAtom : atom_t {
    ERR_ATOM_START = 0xFFFFFF00,
    ERR_ATOM_ANY_DECLARATION = 0xFFFFFF00,
    ERR_ATOM_ANY_IDENTIFIER,
    ERR_ATOM_EOF,
};

void add_error(Error* err);
