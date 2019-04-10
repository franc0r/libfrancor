cmake_minimum_required (VERSION 3.7.2)

project("libfrancor")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      /usr/local/share/cmake
                      /usr/share/cmake)

add_compile_options(-Wall -O2)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(francor_base)
add_subdirectory(francor_algorithm)
add_subdirectory(francor_processing)
add_subdirectory(francor_vision)
add_subdirectory(example)

install(EXPORT francor-config DESTINATION share/cmake/francor)      