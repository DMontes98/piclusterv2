# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/clusternfs/testing/rt/base-rt-app

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/clusternfs/testing/rt/base-rt-app/build

# Include any dependencies generated for this target.
include CMakeFiles/base_rt_app.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/base_rt_app.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/base_rt_app.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/base_rt_app.dir/flags.make

CMakeFiles/base_rt_app.dir/src/main.c.o: CMakeFiles/base_rt_app.dir/flags.make
CMakeFiles/base_rt_app.dir/src/main.c.o: /home/pi/clusternfs/testing/rt/base-rt-app/src/main.c
CMakeFiles/base_rt_app.dir/src/main.c.o: CMakeFiles/base_rt_app.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/clusternfs/testing/rt/base-rt-app/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/base_rt_app.dir/src/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/base_rt_app.dir/src/main.c.o -MF CMakeFiles/base_rt_app.dir/src/main.c.o.d -o CMakeFiles/base_rt_app.dir/src/main.c.o -c /home/pi/clusternfs/testing/rt/base-rt-app/src/main.c

CMakeFiles/base_rt_app.dir/src/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/base_rt_app.dir/src/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/pi/clusternfs/testing/rt/base-rt-app/src/main.c > CMakeFiles/base_rt_app.dir/src/main.c.i

CMakeFiles/base_rt_app.dir/src/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/base_rt_app.dir/src/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/pi/clusternfs/testing/rt/base-rt-app/src/main.c -o CMakeFiles/base_rt_app.dir/src/main.c.s

# Object files for target base_rt_app
base_rt_app_OBJECTS = \
"CMakeFiles/base_rt_app.dir/src/main.c.o"

# External object files for target base_rt_app
base_rt_app_EXTERNAL_OBJECTS =

base_rt_app: CMakeFiles/base_rt_app.dir/src/main.c.o
base_rt_app: CMakeFiles/base_rt_app.dir/build.make
base_rt_app: CMakeFiles/base_rt_app.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/clusternfs/testing/rt/base-rt-app/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable base_rt_app"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/base_rt_app.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/base_rt_app.dir/build: base_rt_app
.PHONY : CMakeFiles/base_rt_app.dir/build

CMakeFiles/base_rt_app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/base_rt_app.dir/cmake_clean.cmake
.PHONY : CMakeFiles/base_rt_app.dir/clean

CMakeFiles/base_rt_app.dir/depend:
	cd /home/pi/clusternfs/testing/rt/base-rt-app/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/clusternfs/testing/rt/base-rt-app /home/pi/clusternfs/testing/rt/base-rt-app /home/pi/clusternfs/testing/rt/base-rt-app/build /home/pi/clusternfs/testing/rt/base-rt-app/build /home/pi/clusternfs/testing/rt/base-rt-app/build/CMakeFiles/base_rt_app.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/base_rt_app.dir/depend

