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




add_executable(kodo
    main.cpp
    InputFile.cpp
    error.cpp
    atom.cpp
    ast.cpp
    Scope.cpp
    node_print.cpp
    type.cpp
    lexer.cpp
    parser.cpp
    NumberLiteral.cpp
    keywords.gperf.gen.inc)

target_include_directories(kodo PRIVATE include)
set_property(TARGET kodo PROPERTY CXX_STANDARD 20)

find_package(Threads REQUIRED)
target_link_libraries(kodo Threads::Threads)
