cmake_minimum_required (VERSION 3.7.2)

project(francor-base VERSION 0.1
                     LANGUAGES CXX)

find_package(Eigen3 REQUIRED NO_MODULE)

add_library(${PROJECT_NAME} INTERFACE
  
)

target_include_directories(${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/francor_base/include/>
            $<INSTALL_INTERFACE:include/>
)

target_link_libraries(${PROJECT_NAME}
  INTERFACE Eigen3::Eigen
)

enable_testing()

add_subdirectory(test)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        RUNTIME DESTINATION bin/francor
        LIBRARY DESTINATION lib/francor
        ARCHIVE DESTINATION lib/francor)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/francor_base/include/francor_base
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

install(EXPORT ${PROJECT_NAME} DESTINATION lib/francor)        