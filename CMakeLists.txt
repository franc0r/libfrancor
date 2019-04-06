cmake_minimum_required (VERSION 3.7.2)

project("libfrancor")

add_compile_options(-Wall -O2)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(francor_base)
add_subdirectory(francor_algorithm)
add_subdirectory(francor_processing)
add_subdirectory(francor_vision)