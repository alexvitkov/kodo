#include <lexer.h>
#include <error.h>
#include <iostream>
#include <vector>

#include <string.h>
#define register
#include "keywords.gperf.gen.inc"
#undef register

#define CT_NONE       0x00
#define CT_LETTER     0x01
#define CT_DIGIT      0x02
#define CT_WHITESPACE 0x04
#define CT_OPERATOR   0x08
#define CT_MULTICHAR_OP_START 0x10

static u8 char_traits[128] = {
/* 0x00      */ CT_NONE,
/* 0x01      */ CT_NONE,
/* 0x02      */ CT_NONE,
/* 0x03      */ CT_NONE,
/* 0x04      */ CT_NONE,
/* 0x05      */ CT_NONE,
/* 0x06      */ CT_NONE,
/* 0x07      */ CT_NONE,
/* 0x08      */ CT_NONE,
/* 0x09 '\t' */ CT_WHITESPACE,
/* 0x0a '\n' */ CT_WHITESPACE,
/* 0x0b      */ CT_NONE,
/* 0x0c      */ CT_NONE,
/* 0x0d '\r' */ CT_WHITESPACE,
/* 0x0e      */ CT_NONE,
/* 0x0f      */ CT_NONE,
/* 0x10      */ CT_NONE,
/* 0x11      */ CT_NONE,
/* 0x12      */ CT_NONE,
/* 0x13      */ CT_NONE,
/* 0x14      */ CT_NONE,
/* 0x15      */ CT_NONE,
/* 0x16      */ CT_NONE,
/* 0x17      */ CT_NONE,
/* 0x18      */ CT_NONE,
/* 0x19      */ CT_NONE,
/* 0x1a      */ CT_NONE,
/* 0x1b      */ CT_NONE,
/* 0x1c      */ CT_NONE,
/* 0x1d      */ CT_NONE,
/* 0x1e      */ CT_NONE,
/* 0x1f      */ CT_NONE,
/* 0x20 ' '  */ CT_WHITESPACE,
/* 0x21 '!'  */ CT_NONE,
/* 0x22 '"'  */ CT_NONE,
/* 0x23 '#'  */ CT_NONE,
/* 0x24 '$'  */ CT_NONE,
/* 0x25 '%'  */ CT_NONE,
/* 0x26 '&'  */ CT_NONE,
/* 0x27 "'"  */ CT_NONE,
/* 0x28 '('  */ CT_OPERATOR,
/* 0x29 ')'  */ CT_OPERATOR,
/* 0x2a '*'  */ CT_NONE,
/* 0x2b '+'  */ CT_MULTICHAR_OP_START | CT_OPERATOR,
/* 0x2c ','  */ CT_OPERATOR,
/* 0x2d '-'  */ CT_MULTICHAR_OP_START | CT_OPERATOR,
/* 0x2e '.'  */ CT_NONE,
/* 0x2f '/'  */ CT_NONE,
/* 0x30 '0'  */ CT_DIGIT,
/* 0x31 '1'  */ CT_DIGIT,
/* 0x32 '2'  */ CT_DIGIT,
/* 0x33 '3'  */ CT_DIGIT,
/* 0x34 '4'  */ CT_DIGIT,
/* 0x35 '5'  */ CT_DIGIT,
/* 0x36 '6'  */ CT_DIGIT,
/* 0x37 '7'  */ CT_DIGIT,
/* 0x38 '8'  */ CT_DIGIT,
/* 0x39 '9'  */ CT_DIGIT,
/* 0x3a ':'  */ CT_OPERATOR,
/* 0x3b ';'  */ CT_NONE,
/* 0x3c '<'  */ CT_NONE,
/* 0x3d '='  */ CT_NONE,
/* 0x3e '>'  */ CT_NONE,
/* 0x3f '?'  */ CT_NONE,
/* 0x40 '@'  */ CT_NONE,
/* 0x41 'A'  */ CT_LETTER,
/* 0x42 'B'  */ CT_LETTER,
/* 0x43 'C'  */ CT_LETTER,
/* 0x44 'D'  */ CT_LETTER,
/* 0x45 'E'  */ CT_LETTER,
/* 0x46 'F'  */ CT_LETTER,
/* 0x47 'G'  */ CT_LETTER,
/* 0x48 'H'  */ CT_LETTER,
/* 0x49 'I'  */ CT_LETTER,
/* 0x4a 'J'  */ CT_LETTER,
/* 0x4b 'K'  */ CT_LETTER,
/* 0x4c 'L'  */ CT_LETTER,
/* 0x4d 'M'  */ CT_LETTER,
/* 0x4e 'N'  */ CT_LETTER,
/* 0x4f 'O'  */ CT_LETTER,
/* 0x50 'P'  */ CT_LETTER,
/* 0x51 'Q'  */ CT_LETTER,
/* 0x52 'R'  */ CT_LETTER,
/* 0x53 'S'  */ CT_LETTER,
/* 0x54 'T'  */ CT_LETTER,
/* 0x55 'U'  */ CT_LETTER,
/* 0x56 'V'  */ CT_LETTER,
/* 0x57 'W'  */ CT_LETTER,
/* 0x58 'X'  */ CT_LETTER,
/* 0x59 'Y'  */ CT_LETTER,
/* 0x5a 'Z'  */ CT_LETTER,
/* 0x5b '['  */ CT_NONE,
/* 0x5c '\'  */ CT_NONE,
/* 0x5d ']'  */ CT_NONE,
/* 0x5e '^'  */ CT_NONE,
/* 0x5f '_'  */ CT_LETTER,
/* 0x60 '`'  */ CT_NONE,
/* 0x61 'a'  */ CT_LETTER,
/* 0x62 'b'  */ CT_LETTER,
/* 0x63 'c'  */ CT_LETTER,
/* 0x64 'd'  */ CT_LETTER,
/* 0x65 'e'  */ CT_LETTER,
/* 0x66 'f'  */ CT_LETTER,
/* 0x67 'g'  */ CT_LETTER,
/* 0x68 'h'  */ CT_LETTER,
/* 0x69 'i'  */ CT_LETTER,
/* 0x6a 'j'  */ CT_LETTER,
/* 0x6b 'k'  */ CT_LETTER,
/* 0x6c 'l'  */ CT_LETTER,
/* 0x6d 'm'  */ CT_LETTER,
/* 0x6e 'n'  */ CT_LETTER,
/* 0x6f 'o'  */ CT_LETTER,
/* 0x70 'p'  */ CT_LETTER,
/* 0x71 'q'  */ CT_LETTER,
/* 0x72 'r'  */ CT_LETTER,
/* 0x73 's'  */ CT_LETTER,
/* 0x74 't'  */ CT_LETTER,
/* 0x75 'u'  */ CT_LETTER,
/* 0x76 'v'  */ CT_LETTER,
/* 0x77 'w'  */ CT_LETTER,
/* 0x78 'x'  */ CT_LETTER,
/* 0x79 'y'  */ CT_LETTER,
/* 0x7a 'z'  */ CT_LETTER,
/* 0x7b '{'  */ CT_OPERATOR,
/* 0x7c '|'  */ CT_NONE,
/* 0x7d '}'  */ CT_OPERATOR,
/* 0x7e '~'  */ CT_NONE,
/* 0x7f      */ CT_NONE,
};

std::vector<Token> tokens;

std::ostream& operator<< (std::ostream& o, const Token& t) {
    o << t.atom;
    return o;
}

void emit(Atom atom) {
    std::cout << atom << "\n";
    tokens.push_back({
        .atom = atom,
    });
}


// buffer must be zero-terminated
bool lex(char* buffer) {
    
    char* word_start = nullptr;
    u8 word_type = 0; // CT_LETTER or CT_DIGIT

    for (char* c = buffer;; c++) {
        if (*c < 0) {
            ERROR("Unsupported character " << (int)*c);
            return false;
        }

        u8 ct = char_traits[*c];

        if (ct & (CT_LETTER | CT_DIGIT)) {
            if (!word_start) {
                word_start = c;
                word_type = ct;
            }
            continue;
        } else if (word_start) {
            if (word_type == CT_LETTER) {
                Atom atom = string_to_atom(word_start, c - word_start);
                emit(atom);
            } else {
                ERROR("numbers not implemented");
            }
            word_start = nullptr;
        }

        if (ct & CT_MULTICHAR_OP_START) {
            for (int i = 5; i > 1; i--) {
                // FIXME buffer overflow
                tok* t = in_word_set(c, i);
                if (t) {
                    c += i - 1;
                    emit(t->atom);
                    goto Next;
                }
            }
        }

        if (ct & CT_OPERATOR) {
            emit(*c);
            continue;
        }

        if (ct == CT_WHITESPACE)
            continue;

        if (*c == '\0')
            return true;

        ERROR("Unsupported character " << (int)*c);
Next:;
    }
}
