cmake_minimum_required (VERSION 3.7.2)

project(examples VERSION 0.1
                 LANGUAGES CXX)

# find blue lines in image examle                 
add_executable(find-blue-lines-in-image
  src/find_blue_lines_in_image.cpp
)                 

target_link_libraries(find-blue-lines-in-image
  PRIVATE francor-processing
)

# simulate laser scans example
add_executable(simulate-laser-scans
  src/simulate_laser_scans.cpp
)

target_link_libraries(simulate-laser-scans
  PRIVATE francor-mapping
)