#pragma once

#include <iosfwd>
#include <common.h>

enum TokenType : _atom_t {
    TOK_NONE = 0,

    TOK_COMMA = ',',
    TOK_COLON = ':',

    TOK_OPENCURLY = '{',
    TOK_CLOSECURLY = '}',
    TOK_OPENBRACKET = '(',
    TOK_CLOSEBRACKET = ')',
    TOK_OPENSQUARE = '[',
    TOK_CLOSESQUARE = ']',

    // TOK_IDENTIFIER = 128,
    // TOK_NUMBER_LITERAL = 129,
};


struct Token {
    Atom atom;
    inline operator bool() { return atom; }
    inline bool is_identifier() { return atom.is_identifier(); }
    inline operator _atom_t() { return atom.atom; }
};

extern Atom KW_FN;

std::ostream& operator<< (std::ostream& o, const Token& t);
