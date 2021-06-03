#include <iostream>
#include <dirent.h>

#include <Node.h>
#include <common.h>
#include <GlobalContext.h>

const std::string parser_tests = "test/parser_tree_compare";


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


void begin_test(std::string name) {
    current_test = new Test();
    current_test->name = name;
    current_test->state = Test::RUNNING;
    current_test->global = new GlobalContext();
    global = current_test->global;

    tests.push_back(current_test);

    std::cout << name << "... ";
}

void fail_test() {
    std::cout << "FAILED\n";
    current_test->state = Test::FAILED;
}

void pass_test() {
    std::cout << "PASSED\n";
    current_test->state = Test::PASSED;
}


void run_tree_compare_tests() {
    DIR* d = opendir(parser_tests.c_str());
    dirent* ent;

    if (!d) {
        std::cout << "Failed to open directory '" << parser_tests << "'. These tests will not be run\n";
        return;
    }

    while ((ent = readdir(d))) {
        if (ent->d_name[0] == '.')
            continue;

        GlobalContext _global;
        global = &_global;

        std::string filename = parser_tests + "/" + ent->d_name;
        std::string test_name = filename;
        begin_test(test_name);

        InputFile* input = global->add_source(filename);
        if (!input) {
            fail_test();
            continue;
        }
        if (!input->lex()) {
            fail_test();
            continue;
        }
        if (!input->parse()) {
            fail_test();
            continue;
        }

        if (global->scope->statements.size() != 2) {
            fail_test();
            continue;
        }

        Scope* s1 = dynamic_cast<Scope*>(global->scope->statements[0]);
        Scope* s2 = dynamic_cast<Scope*>(global->scope->statements[1]);

        if (!s1 || !s2) {
            fail_test();
            continue;
        }

        if (!s1->tree_compare(s2)) {
            fail_test();
            continue;
        }

        pass_test();
    }

    closedir(d);
}


int main() {
    run_tree_compare_tests();

    std::cout << "\n";

    int failed_tests = 0;
    for (Test* t : tests) {
        if (t->state != Test::PASSED) {
            failed_tests++;
            std::cout << t->name << " errors:\n";
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
