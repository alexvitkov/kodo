cmake_minimum_required(VERSION 3.20)
project(kodo)

add_executable(kodo
    main.cpp
    error.cpp
    atom.cpp
    lexer.cpp
    parser.cpp
    ast.cpp)
target_include_directories(kodo PRIVATE include)
set_property(TARGET kodo PROPERTY CXX_STANDARD 20)

find_package(Threads REQUIRED)
target_link_libraries(kodo Threads::Threads)
