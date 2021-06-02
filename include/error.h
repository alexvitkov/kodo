#pragma once

#include <iostream>
#include <Token.h>


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

struct UnsupportedCharacterError : Error {
    char character;
    inline UnsupportedCharacterError(char ch) : character(ch) {}
    virtual void print() override;
};

struct InvalidTokenError : Error {
    std::string tok;
    virtual void print() override;
    inline InvalidTokenError(std::string tok) : tok(tok) {}
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
