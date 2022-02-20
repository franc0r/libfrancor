cmake_minimum_required (VERSION 3.7.2)

project(francor-can VERSION 0.1
                       LANGUAGES CXX)
                       
add_library(${PROJECT_NAME} SHARED
  src/socketcan.cpp  
)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
         $<INSTALL_INTERFACE:include/>
)

enable_testing()

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT francor-config
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_can/include/francor_can
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")
