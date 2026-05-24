cmake_minimum_required(VERSION 3.10)
project(SimpleOSSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simpleos_system_x86 system.cpp system_x86.cpp)
add_executable(simpleos_system_x64 system.cpp system_x64.cpp)

if (MSVC)
    target_compile_options(simpleos_system_x86 PRIVATE /W4)
    target_compile_options(simpleos_system_x64 PRIVATE /W4)
else()
    target_compile_options(simpleos_system_x86 PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(simpleos_system_x64 PRIVATE -Wall -Wextra -Wpedantic)
endif()
