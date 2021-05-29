#include <error.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <initializer_list>
#include <iostream>

struct AtomGroup {
    int start;
    int next;
    std::unordered_map<std::string, Atom> _forward;
    std::vector<std::string> _reverse;

    AtomGroup(int start) : start(start), next(start) {}

    Atom find_or_add(const char* str, int length) {
        std::string s(str, length);
        auto it = _forward.find(s);

        if (it == _forward.end()) {
            _atom_t atom = next;
            next++;
            _forward.insert({ s, atom });
            _reverse.push_back(s);
            return atom;
            return 0;
        } else {
            return it->second;
        }
    }

    Atom find(const char* str, int length) {
        std::string s(str, length);
        auto it = _forward.find(s);
        if (it == _forward.end()) {
            return 0;
        } else {
            return it->second;
        }
    }

    bool find_reverse(Atom a, std::string& s) {
        if (a.atom >= start && a.atom < next) {
            s = _reverse[a.atom - start];
            return true;
        } else {
            return false;
        }
    }
};

AtomGroup keywords(0x100);
AtomGroup user(0x1000);

Atom KW_FN;

void init_keywords() {
     KW_FN = keywords.find_or_add("fn", 2);
}

Atom string_to_atom(const char* str, int length) {
    // FIXME THREADSAFETY
    Atom a = keywords.find(str, length);
    if (a) 
        return a;

    return user.find_or_add(str, length);
}

std::string atom_to_string(u32 atom) {
    // FIXME THREADSAFETY
    std::string s = "INVALID_ATOM";

    if (keywords.find_reverse(atom, s)) return s;
    if (user.find_reverse(atom, s)) return s;
    return s;
}

std::ostream& operator<< (std::ostream& o, Atom p) {
    if (p.atom < 128) {
        o << '\'' << (char)p.atom << '\'';
    } 
    else if (p.atom >= ERR_ATOM_START) {
        switch ((ErrorAtom)p.atom) {
            case ERR_ATOM_ANY_DECLARATION: o << "a declaration"; break;
            case ERR_ATOM_EOF:             o << "end of file"; break;
            case ERR_ATOM_ANY_IDENTIFIER:  o << "an identifier"; break;
        }
    }
    else {
        o << atom_to_string(p.atom);
    }
    return o;
}

bool Atom::is_identifier() {
    return atom >= user.start && atom < user.next;
}
