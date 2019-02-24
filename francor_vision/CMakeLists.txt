cmake_minimum_required (VERSION 3.7.2)

project(francor-vision VERSION 0.1
                       LANGUAGES CXX)

find_package(OpenCV REQUIRED core imgproc)
                       
add_library(${PROJECT_NAME}
  src/image.cpp  
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${OpenCV_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:include/>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC ${OpenCV_LIBS}
)

add_subdirectory(test)