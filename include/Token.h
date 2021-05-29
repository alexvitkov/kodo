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
    TOK_FN = 0x80,
    TOK_PLUSPLUS,
    TOK_MINUSMINUS,


    // TOK_IDENTIFIER = 128,
    // TOK_NUMBER_LITERAL = 129,
};


struct Token {
    Atom atom;
    inline bool is_identifier() { return atom.is_identifier(); }

    inline operator bool() { return (bool)atom; }
    inline operator Atom() { return atom; }
    inline operator atom_t() { return atom.atom; }
};

std::ostream& operator<< (std::ostream& o, const Token& t);
