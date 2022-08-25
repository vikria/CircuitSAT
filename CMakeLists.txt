cmake_minimum_required(VERSION 3.23)
project(CircuitSAT)

ADD_COMPILE_OPTIONS(-Wall -Werror -Wextra -pedantic)

set(CMAKE_CXX_STANDARD 17)

add_executable(CircuitSAT   ./source/main.cpp
                            ./source/Circuit_parse.cpp
                            ./source/Circuit_simplify.cpp
                            ./source/Circuit_solve.cpp
                            ./source/Circuit_show_result.cpp
                            ./source/Operators.cpp
        )