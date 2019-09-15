cmake_minimum_required(VERSION 3.0)
project(GlobalKeyProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Win32 Unicode
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Subdirectories
add_subdirectory(GlobalKeyLib)
add_subdirectory(GlobalKeyApp)
