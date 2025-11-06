cmake_minimum_required(VERSION 3.10)

# Set C++ compiler to g++
set(CMAKE_CXX_COMPILER "g++")

project(MatroidIntersection)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add include directory
include_directories(include)

# Fetch nlohmann/json library
include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Source files
set(SOURCES
    src/matroid_problem.cpp
    src/matroid_intersection.cpp
    src/matroid_implementations.cpp
    src/graph_generator.cpp
    src/validation.cpp
)

# Create executable
add_executable(matroid_intersection src/main.cpp ${SOURCES})

# Link nlohmann/json (header-only, but ensures proper include path)
target_link_libraries(matroid_intersection PRIVATE nlohmann_json::nlohmann_json)

# Enable warnings
if(MSVC)
    target_compile_options(matroid_intersection PRIVATE /W4)
else()
    target_compile_options(matroid_intersection PRIVATE -Wall -Wextra -pedantic)
endif()

# Install target
install(TARGETS matroid_intersection DESTINATION bin)
