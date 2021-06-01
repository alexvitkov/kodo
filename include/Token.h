#pragma once

#include <iosfwd>
#include <common.h>

enum TokenType : atom_t {
    TOK_NONE = 0,

    TOK_COMMA = ',',
    TOK_COLON = ':',

    TOK_OPENCURLY = '{',
    TOK_CLOSECURLY = '}',
    TOK_OPENBRACKET = '(',
    TOK_CLOSEBRACKET = ')',
    TOK_OPENSQUARE = '[',
    TOK_CLOSESQUARE = ']',

    // VOLATILE - must be ordered the same as the string array in atom.cpp
    TOK_I8 = 0x80,
    TOK_I16,
    TOK_I32,
    TOK_I64,
    TOK_U8,
    TOK_U16,
    TOK_U32,
    TOK_U64,
    TOK_NUMBER_LITERAL,
    TOK_TYPE,
    TOK_FN,
    TOK_PLUSPLUS,
    TOK_MINUSMINUS,


    // TOK_IDENTIFIER = 128,
    // TOK_NUMBER_LITERAL = 129,
};


struct Token : Atom {
    struct NumberLiteral* nl;

    inline Token(Atom atom) : Atom(atom) {}
    inline Token(atom_t at) : Atom(at) {}

    static inline Token number_literal(NumberLiteral* nl) { Token t(TOK_NUMBER_LITERAL); t.nl = nl; return t; };
};
