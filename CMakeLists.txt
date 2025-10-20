cmake_minimum_required(VERSION 4.0)
project(varia)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(varia example.cpp)

if (MSVC)
    target_compile_options(varia PRIVATE /W4 /permissive- /Zc:preprocessor /Zc:__cplusplus /EHsc)
else ()
    target_compile_options(varia PRIVATE -Wall -Wextra -Wpedantic)
endif ()


