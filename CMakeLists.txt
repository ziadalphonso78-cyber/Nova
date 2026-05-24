cmake_minimum_required(VERSION 3.14)
project(NovaLanguage VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/main.cpp
    src/core/token.cpp
    src/core/lexer.cpp
    src/core/parser.cpp
    src/runtime/value.cpp
    src/runtime/vm.cpp
)

# Header files
set(HEADERS
    src/core/token.h
    src/core/lexer.h
    src/core/ast.h
    src/core/parser.h
    src/runtime/value.h
    src/runtime/vm.h
)

# Add executable
add_executable(nova ${SOURCES} ${HEADERS})

# Compiler flags
if(MSVC)
    target_compile_options(nova PRIVATE /W4)
else()
    target_compile_options(nova PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Include directories
target_include_directories(nova PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)