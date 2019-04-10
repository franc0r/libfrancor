cmake_minimum_required (VERSION 3.7.2)

project(francor-vision VERSION 0.1
                       LANGUAGES CXX)

find_package(OpenCV REQUIRED core imgproc imgcodecs)
                       
add_library(${PROJECT_NAME} SHARED
  src/image.cpp  
)

target_include_directories(${PROJECT_NAME}
  PUBLIC ${OpenCV_INCLUDE_DIRS}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
         $<INSTALL_INTERFACE:include/>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC ${OpenCV_LIBS}
)

enable_testing()

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_vision/include/francor_vision
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

# install(EXPORT ${PROJECT_NAME}-config DESTINATION lib/cmake)