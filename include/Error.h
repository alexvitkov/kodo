#pragma once

#include <vector>
#include <iostream>
#include <Token.h>


struct Error {
    virtual void print() = 0;
};

struct FailedToReadInputFileError : Error {
    std::string path;
    inline FailedToReadInputFileError(std::string path) : path(path) {}
    virtual void print() override;
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

struct AlreadyDefinedError : Error {
    std::vector<struct Node*> definitions;
    AlreadyDefinedError(std::vector<Node*> nodes) : definitions(nodes) {}
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

struct NotATypeError : Error {
    Node* node;
    virtual void print() override;
    inline NotATypeError(Node* node) : node(node) {}
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
