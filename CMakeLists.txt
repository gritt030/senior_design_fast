cmake_minimum_required(VERSION 2.6)
project(quadcopter)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pg")

add_executable(quadcopter main.cpp)

install(TARGETS quadcopter RUNTIME DESTINATION bin)

add_subdirectory(occupancygrid)
add_subdirectory(localization)
add_subdirectory(sonar)
add_subdirectory(linefitter)
add_subdirectory(ppmwriter)

target_link_libraries(quadcopter localizer line_fitter occupancy_grid sonar ppm_writer)

