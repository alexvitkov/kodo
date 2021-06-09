cmake_minimum_required(VERSION 3.20)
project(kodo)


# If we have gperf installed we can regenerate the keywords.gperf.gen.inc on need
find_program(HAS_GPERF "gperf")
if (HAS_GPERF)
    add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/keywords.gperf.gen.inc
        COMMAND gperf ${CMAKE_SOURCE_DIR}/keywords.gperf --output-file=${CMAKE_SOURCE_DIR}/keywords.gperf.gen.inc
        MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/keywords.gperf)
else()
    message(WARNING "gperf not found, keywords.gperf.gen.inc won't be generated if you modify the keywords.gperf file")
endif()


set(SOURCES
    InputFile.cpp
    error.cpp
    atom.cpp
    ast.cpp
    Scope.cpp
    node_clone.cpp
    node_print.cpp
    type.cpp
    lexer.cpp
    parser.cpp
    NumberLiteral.cpp
    Function.cpp
    Interpreter.cpp
    keywords.gperf.gen.inc)


add_executable(kodo
    main.cpp
    ${SOURCES})

add_executable(kodo_test
    test/test_main.cpp
    node_tree_compare.cpp
    ${SOURCES})

target_include_directories(kodo      PRIVATE include)
target_include_directories(kodo_test PRIVATE include)
set_property(TARGET kodo      PROPERTY CXX_STANDARD 20)
set_property(TARGET kodo_test PROPERTY CXX_STANDARD 20)

target_compile_definitions(kodo_test PUBLIC _TEST)

find_package(Threads REQUIRED)
target_link_libraries(kodo Threads::Threads)
