#include <iostream>
#include <iomanip>
#include <dirent.h>

#include <Node.h>
#include <common.h>
#include <GlobalContext.h>


#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>


const std::string tree_compare_tests = "test/parser_tree_compare";
const std::string must_parse_tests = "test/must_parse";
const std::string must_resolve_tests = "test/must_resolve";
const std::string must_not_resolve_tests = "test/must_not_resolve";
const std::string must_not_parse_tests = "test/must_not_parse";


struct Test {
    enum State {
        RUNNING,
        FAILED,
        PASSED
    } state;
    std::string name;
    GlobalContext* global;
};

std::vector<Test*> tests;

Test* current_test;
GlobalContext* global;

int terminal_width;

void begin_test(std::string name) {
    current_test = new Test();
    current_test->name = name;
    current_test->state = Test::RUNNING;
    current_test->global = new GlobalContext();
    global = current_test->global;

    tests.push_back(current_test);

    

    std::cout << std::left << std::setw(terminal_width - 6) << name;
}

void fail_test() {
    std::cout << "FAILED\n";
    current_test->state = Test::FAILED;
}

void pass_test() {
    std::cout << "PASSED\n";
    current_test->state = Test::PASSED;
}


template <typename Fn>
void run_test_in_directory(std::string dir, Fn fn) {
    DIR* d = opendir(dir.c_str());
    dirent* ent;

    if (!d) {
        std::cout << "Failed to open directory '" << tree_compare_tests << "'. These tests will not be run\n";
        return;
    }

    while ((ent = readdir(d))) {
        if (ent->d_name[0] == '.')
            continue;

        GlobalContext _global;
        global = &_global;

        std::string filename = dir + "/" + ent->d_name;
        std::string test_name = filename;
        begin_test(test_name);

        InputFile* input = global->add_source(filename);
        if (!input) {
            fail_test();
            continue;
        }

        if (!fn(input)) {
            fail_test();
        } else {
            pass_test();
        }
    }

    closedir(d);
}


bool run_tree_compare_test(InputFile* input) {
    MUST (input->lex());
    MUST (input->parse());

    MUST (global->scope->statements.size() == 2);

    Scope* s1 = dynamic_cast<Scope*>(global->scope->statements[0]);
    Scope* s2 = dynamic_cast<Scope*>(global->scope->statements[1]);

    MUST (s1 && s2);
    MUST (s1->tree_compare(s2));

    return true;
}

bool run_must_parse_test(InputFile* input) {
    MUST (input->lex());
    MUST (input->parse());
    return true;
}

bool run_must_not_parse_test(InputFile* input) {
    MUST (input->lex());
    MUST (!input->parse());
    return true;
}

bool run_must_resolve_test(InputFile* input) {
    MUST (input->lex());
    MUST (input->parse());
    MUST (global->scope->forward_declare_pass(nullptr));
    MUST (global->scope->resolve_pass(nullptr, nullptr, nullptr));
    return true;
}

bool run_must_not_resolve_test(InputFile* input) {
    MUST (input->lex());
    MUST (input->parse());
    MUST (global->scope->forward_declare_pass(nullptr));
    MUST (!global->scope->resolve_pass(nullptr, nullptr, nullptr));
    return true;
}

int main() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    terminal_width = w.ws_col;


    run_test_in_directory(tree_compare_tests, run_tree_compare_test);
    run_test_in_directory(must_parse_tests, run_must_parse_test);
    run_test_in_directory(must_parse_tests, run_must_parse_test);
    run_test_in_directory(must_resolve_tests, run_must_resolve_test);
    run_test_in_directory(must_not_resolve_tests, run_must_not_resolve_test);
    run_test_in_directory(must_not_parse_tests, run_must_not_parse_test);

    std::cout << "\n";

    int failed_tests = 0;
    for (Test* t : tests) {
        if (t->state != Test::PASSED) {
            failed_tests++;

            std::cout << t->name << " failed.";

            if (!t->global->errors.empty())
                std::cout << "Errors: ";
            for (Error* err : t->global->errors) {
                err->print();
                std::cout << "\n";
            }
            std::cout << "\n";
        }
    }

    std::cout << tests.size() - failed_tests << "/" << tests.size() << " tests passed.\n";

    return 0;
}
