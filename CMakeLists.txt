cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(fdupl LANGUAGES CXX)

add_subdirectory(lib)
add_subdirectory(tests)
