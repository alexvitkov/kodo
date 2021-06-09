#include <Node/NumberLiteral.h>
#include <iostream>


Type* NumberLiteral::get_type() { 
    return &t_number_literal; 
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

NumberLiteralToPrimitiveCast::NumberLiteralToPrimitiveCast(Type* _destination_type) {
    this->destination_type = _destination_type;
    this->source_type = &t_number_literal;
}


Node* NumberLiteralToPrimitiveCast::get_node(Node* source) {
    NumberLiteral* nl = (NumberLiteral*)source;

    RuntimeValue* casted = new RuntimeValue();

    casted->type = destination_type;
    casted->int_value = 0;

    for (int i = 0; i < nl->digits.size(); i++) {
        casted->int_value *= 10;
        casted->int_value += nl->digits[i];
    }

    if (nl->negative)
        casted->int_value = -casted->int_value;

    return casted;
}

RuntimeValue* NumberLiteral::evaluate(Interpreter* interpreter) {
    RuntimeValue* val = new RuntimeValue();
    val->type = &t_number_literal;
    val->data = this;
    return val;
}
