cmake_minimum_required (VERSION 3.7.2)

project(examples VERSION 0.1
                 LANGUAGES CXX)

add_executable(find-blue-lines-in-image
  src/find_blue_lines_in_image.cpp
)                 

target_link_libraries(find-blue-lines-in-image
  PRIVATE francor-processing
)