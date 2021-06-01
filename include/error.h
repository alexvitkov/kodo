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

struct InvalidDeclarationError : Error {
    struct Call* declaration;
    inline InvalidDeclarationError(struct Call* decl) : declaration(decl) {}
    virtual void print() override;
};

struct InvalidCallError : Error {
    Call* call;
    inline InvalidCallError(struct Call* call) : call(call) {}
    virtual void print() override;
};

enum ErrorAtom : atom_t {
    ERR_ATOM_START = 0xFFFFFF00,
    ERR_ATOM_ANY_DECLARATION = 0xFFFFFF00,
    ERR_ATOM_ANY_IDENTIFIER,
    ERR_ATOM_EOF,
    ERR_ATOM_AN_ARGUMENT,
    ERR_ATOM_ANY_EXPRESSION,
};

void add_error(Error* err);
