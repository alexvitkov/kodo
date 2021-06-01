#include <NumberLiteral.h>
#include <iostream>


void NumberLiteral::print(std::ostream& o, bool print_definition) {
    if (digits.size() == 0) {
        o << '0';
        return;
    }

    if (negative)
        o << '-';

    for (int i = 0; i < digits.size(); i++)
        o << (int)digits[i];
}

