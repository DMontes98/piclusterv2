# Define Minimum CMake Version Requred to build project
cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 11)

# Define Project
project(BaseRTApp)

# Define Source Files
file(GLOB_RECURSE sources src/*.c src/*.h)

# Add Executable
add_executable(base_rt_app ${sources})

#Link Libraries
target_link_libraries(base_rt_app PUBLIC cap)

# Add some compiler flags
target_compile_options(base_rt_app PUBLIC -Wall -lpthread)