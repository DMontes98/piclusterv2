# Define Minimum CMake Version Requred to build project
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 11)

# Define Project
project(MulticoreRTApp)

# Define Source Files
file(GLOB_RECURSE sources src/*.c src/*.h)

# Add Executable
add_executable(multicore_rt_app ${sources})

# Include Directories
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

# Find Packages
find_package(MPI REQUIRED)

#Link Libraries
target_link_libraries(multicore_rt_app PUBLIC cap)
target_link_libraries(multicore_rt_app PUBLIC MPI::MPI_CXX)

# Add some compiler flags
target_compile_options(multicore_rt_app PUBLIC -Wall -lpthread) 