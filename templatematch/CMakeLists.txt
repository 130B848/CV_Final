# Detect if we want to build samples with library binaries or not
#if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
#
# BUILD CASE 1: Build samples with library sources
#

cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

project( TEMPLATE_MATCH )
find_package( OpenCV REQUIRED )
add_executable( template template.cpp )
target_link_libraries( template ${OpenCV_LIBS} )
