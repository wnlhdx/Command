# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
SHELL = /data/data/com.termux/files/usr/bin/sh

# The CMake executable.
CMAKE_COMMAND = /data/data/com.termux/files/usr/bin/cmake

# The command to remove a file.
RM = /data/data/com.termux/files/usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux"

# Include any dependencies generated for this target.
include CMakeFiles/game.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/game.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/game.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/game.dir/flags.make

CMakeFiles/game.dir/game.cpp.o: CMakeFiles/game.dir/flags.make
CMakeFiles/game.dir/game.cpp.o: /data/data/com.termux/files/home/InnerCode/Command/C&C++/game/game.cpp
CMakeFiles/game.dir/game.cpp.o: CMakeFiles/game.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/game.dir/game.cpp.o"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/game.dir/game.cpp.o -MF CMakeFiles/game.dir/game.cpp.o.d -o CMakeFiles/game.dir/game.cpp.o -c "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/game.cpp"

CMakeFiles/game.dir/game.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/game.dir/game.cpp.i"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/game.cpp" > CMakeFiles/game.dir/game.cpp.i

CMakeFiles/game.dir/game.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/game.dir/game.cpp.s"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/game.cpp" -o CMakeFiles/game.dir/game.cpp.s

# Object files for target game
game_OBJECTS = \
"CMakeFiles/game.dir/game.cpp.o"

# External object files for target game
game_EXTERNAL_OBJECTS =

game: CMakeFiles/game.dir/game.cpp.o
game: CMakeFiles/game.dir/build.make
game: CMakeFiles/game.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable game"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/game.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/game.dir/build: game
.PHONY : CMakeFiles/game.dir/build

CMakeFiles/game.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/game.dir/cmake_clean.cmake
.PHONY : CMakeFiles/game.dir/clean

CMakeFiles/game.dir/depend:
	cd "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/game/linux/CMakeFiles/game.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/game.dir/depend

