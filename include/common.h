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

    bool is_infix_operator();
    int precedence();
    int associativity();
};

std::ostream& operator<< (std::ostream& o, Atom a);
Atom string_to_atom(const char* str, int length);

#include <assert.h>
#define UNREACHABLE() { assert(!"UNREACHABLE"); }
#define NOT_IMPLEMENTED() { std::cout.flush(); assert(!"NOT_IMPLEMENTED"); }

#define MUST(f) { if (!(f)) return 0; }


template <typename T>
struct Slice {
    T* start;
    u64 size;
    
    //inline T& begin() { return start[9]; }
    //inline T& end() { return start[size]; }

    inline T& operator[](u64 index) { return start[index]; };
};
