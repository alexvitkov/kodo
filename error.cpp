#include <iostream>
#include <vector>
#include <mutex>

#include <Error.h>
#include <Node.h>
#include <Node/Call.h>

std::vector<Error*> errors;
std::mutex errors_mutex;

void add_error(Error* err) {
    std::unique_lock<std::mutex> _lock(errors_mutex);
    errors.push_back(err);
    err->print();
    std::cout << "\n\n";
}

void UnexpectedTokenError::print() {
    std::cout << "Unexpected " << actual << " while looking for " << expected;
}

void InvalidDeclarationError::print() {
    std::cout << "InvalidDeclarationError";
}

void InvalidCallError::print() {
    std::cout << "InvalidCallError: " << call;
}

void UnsupportedCharacterError::print() {
    std::cout << "UnsupportedCharacterError: " << (int)character;
}

void InvalidTokenError::print() {
    std::cout << "InvalidTokenError: " << tok;
}

void FailedToReadInputFileError::print() {
    std::cout << "Failed to open input file '" << path << "'";
}

void AlreadyDefinedError::print() {
    std::cout << "Already defined:\n";
    for (Node* d : definitions)
        std::cout << d << "\n";
}

void NotATypeError::print() {
    std::cout << "Expected a type, instead got " << node;
}
