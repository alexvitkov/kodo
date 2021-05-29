#pragma once

#include <stdint.h>
#include <iosfwd>
#include <string>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef u32 atom_t;

struct Atom {
    atom_t atom;

    inline Atom() : atom(0) {}
    inline Atom(atom_t atom) : atom(atom) {}
    inline operator atom_t() { return atom; }

    bool is_identifier();
    inline bool is_infix_operator() { return atom == '+' || atom == '*'; }
};

std::ostream& operator<< (std::ostream& o, Atom a);
Atom string_to_atom(const char* str, int length);

#include <assert.h>
#define UNREACHABLE() { assert(!"UNREACHABLE"); }
#define NOT_IMPLEMENTED() { assert(!"NOT_IMPLEMENTED"); }

#define MUST(f) { if (!(f)) return 0; }
