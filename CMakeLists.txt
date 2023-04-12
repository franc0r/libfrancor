cmake_minimum_required (VERSION 3.7.2)

project(francor)

set(BUILD_OPENCV_COMPONENTS On CACHE BOOL "Build components which depend on OpenCV")
set(BUILD_DRIVE_COMPONENTS On CACHE BOOL "Build components which are necessary for drives/actuation!")

set(BUILD_EXAMPLES On CACHE BOOL "Build examples (all components must be enabled for build!)")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      /usr/local/share/cmake
                      /usr/share/cmake
)

add_compile_options(-Wall)# -O2)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(francor_base)
add_subdirectory(francor_algorithm)
add_subdirectory(francor_processing)

if (BUILD_OPENCV_COMPONENTS)
    add_subdirectory(francor_mapping)
    add_subdirectory(francor_vision)
endif()

if (BUILD_DRIVE_COMPONENTS)
    add_subdirectory(francor_can)
    add_subdirectory(francor_drive)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

install(EXPORT francor-config DESTINATION share/cmake/francor)      