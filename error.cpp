#include <iostream>
#include <vector>
#include <mutex>

#include <error.h>
#include <ast.h>

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
