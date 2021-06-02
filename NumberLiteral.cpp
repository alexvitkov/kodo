#include <ast.h>
#include <NumberLiteral.h>
#include <iostream>


Type* NumberLiteral::get_type() { 
    return &t_number_literal; 
}

Type* CastedNumberLiteral::get_type() { 
    return type;
}


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

void CastedNumberLiteral::print(std::ostream& o, bool print_definition) {
    o << number << type;
}

NumberLiteralToPrimitiveCast::NumberLiteralToPrimitiveCast(Type* _destination_type) {
    this->destination_type = _destination_type;
    this->source_type = &t_number_literal;
}


Node* NumberLiteralToPrimitiveCast::get_node(Node* source) {
    NumberLiteral* nl = (NumberLiteral*)source;

    CastedNumberLiteral* casted = new CastedNumberLiteral();

    casted->type = destination_type;
    casted->number = 0;

    for (int i = 0; i < nl->digits.size(); i++) {
        casted->number *= 10;
        casted->number += nl->digits[i];
    }

    if (nl->negative)
        casted->number = -casted->number;

    return casted;
}
