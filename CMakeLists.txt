cmake_minimum_required(VERSION 3.23)

project(LibHTML)

set(CMAKE_C_FLAGS_DEBUG "-ggdb -fsanitize=address")

set(SOURCES src/ht.c
            src/Attribute.c
            src/Token.c
            src/Tokenizer.c
            src/Parser.c
            src/DOM/Node.c
            src/DOM/NodeStack.c)

add_executable(ht ${SOURCES})
