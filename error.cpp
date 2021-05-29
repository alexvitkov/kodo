#include <iostream>
#include <vector>
#include <mutex>

#include <error.h>

std::vector<Error*> errors;
std::mutex errors_mutex;

void add_error(Error* err) {
    std::unique_lock<std::mutex> _lock(errors_mutex);
    errors.push_back(err);
    err->print();
}

void UnexpectedTokenError::print() {
    std::cout << "Unexpected " << actual << " while looking for " << expected << "\n";
}

void RepeatedArgumentError::print() {
    std::cout << "Repeated Argument";
}
