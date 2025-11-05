cmake_minimum_required(VERSION 3.10)

# Set C++ compiler to g++
set(CMAKE_CXX_COMPILER "g++")

project(MatroidIntersection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Source files
set(SOURCES
    src/matroid_intersection.cpp
    src/matroid_implementations.cpp
    src/graph_generator.cpp
)

# Create executable
add_executable(matroid_intersection src/main.cpp ${SOURCES})

# Enable warnings
if(MSVC)
    target_compile_options(matroid_intersection PRIVATE /W4)
else()
    target_compile_options(matroid_intersection PRIVATE -Wall -Wextra -pedantic)
endif()

# Install target
install(TARGETS matroid_intersection DESTINATION bin)
