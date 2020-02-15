cmake_minimum_required (VERSION 3.7.2)

project(examples VERSION 0.1
                 LANGUAGES CXX)

# find blue lines in image examle                 
add_executable(find-blue-lines-in-image
  src/find_blue_lines_in_image.cpp
)                 

target_link_libraries(find-blue-lines-in-image
  PRIVATE francor-processing
  PRIVATE francor-algorithm
  PRIVATE francor-vision
)

# simulate laser scans example
add_executable(simulate-laser-scans
  src/simulate_laser_scans.cpp
)

target_link_libraries(simulate-laser-scans
  PRIVATE francor-mapping
)

# updating occupancy grid example
add_executable(updating-occupancy-grid
  src/updating_occupancy_grid.cpp
)

target_link_libraries(updating-occupancy-grid
  PRIVATE francor-mapping
)

# localize and update ego example
add_executable(localize-ego-on-occupancy-grid
  src/localize_ego_on_occupancy_grid.cpp
)

target_link_libraries(localize-ego-on-occupancy-grid
  PRIVATE francor-mapping
)

# slam occupancy grid example
add_executable(slam-occupancy-grid
  src/slam_occupancy_grid.cpp
)

target_link_libraries(slam-occupancy-grid
  PRIVATE francor-mapping
)