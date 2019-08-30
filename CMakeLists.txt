cmake_minimum_required(VERSION 3.0)
project(GlobalKeyApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QHOTKEY_EXAMPLES OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-multichar")
endif()

# Subdirectories
add_subdirectory(QHotKey)
add_subdirectory(GlobalKey)
