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
CMAKE_SOURCE_DIR = "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build"

# Include any dependencies generated for this target.
include CMakeFiles/qtgame.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/qtgame.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/qtgame.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/qtgame.dir/flags.make

CMakeFiles/qtgame.dir/main.cpp.o: CMakeFiles/qtgame.dir/flags.make
CMakeFiles/qtgame.dir/main.cpp.o: /data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/main.cpp
CMakeFiles/qtgame.dir/main.cpp.o: CMakeFiles/qtgame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/qtgame.dir/main.cpp.o"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/qtgame.dir/main.cpp.o -MF CMakeFiles/qtgame.dir/main.cpp.o.d -o CMakeFiles/qtgame.dir/main.cpp.o -c "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/main.cpp"

CMakeFiles/qtgame.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/qtgame.dir/main.cpp.i"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/main.cpp" > CMakeFiles/qtgame.dir/main.cpp.i

CMakeFiles/qtgame.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/qtgame.dir/main.cpp.s"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/main.cpp" -o CMakeFiles/qtgame.dir/main.cpp.s

CMakeFiles/qtgame.dir/qtgame.cpp.o: CMakeFiles/qtgame.dir/flags.make
CMakeFiles/qtgame.dir/qtgame.cpp.o: /data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/qtgame.cpp
CMakeFiles/qtgame.dir/qtgame.cpp.o: CMakeFiles/qtgame.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/qtgame.dir/qtgame.cpp.o"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/qtgame.dir/qtgame.cpp.o -MF CMakeFiles/qtgame.dir/qtgame.cpp.o.d -o CMakeFiles/qtgame.dir/qtgame.cpp.o -c "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/qtgame.cpp"

CMakeFiles/qtgame.dir/qtgame.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/qtgame.dir/qtgame.cpp.i"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/qtgame.cpp" > CMakeFiles/qtgame.dir/qtgame.cpp.i

CMakeFiles/qtgame.dir/qtgame.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/qtgame.dir/qtgame.cpp.s"
	/data/data/com.termux/files/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/qtgame.cpp" -o CMakeFiles/qtgame.dir/qtgame.cpp.s

# Object files for target qtgame
qtgame_OBJECTS = \
"CMakeFiles/qtgame.dir/main.cpp.o" \
"CMakeFiles/qtgame.dir/qtgame.cpp.o"

# External object files for target qtgame
qtgame_EXTERNAL_OBJECTS =

qtgame: CMakeFiles/qtgame.dir/main.cpp.o
qtgame: CMakeFiles/qtgame.dir/qtgame.cpp.o
qtgame: CMakeFiles/qtgame.dir/build.make
qtgame: /data/data/com.termux/files/usr/lib/libQt6Widgets.so.6.7.1
qtgame: /data/data/com.termux/files/usr/lib/libQt6Gui.so.6.7.1
qtgame: /data/data/com.termux/files/usr/lib/libQt6Core.so.6.7.1
qtgame: CMakeFiles/qtgame.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable qtgame"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/qtgame.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/qtgame.dir/build: qtgame
.PHONY : CMakeFiles/qtgame.dir/build

CMakeFiles/qtgame.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/qtgame.dir/cmake_clean.cmake
.PHONY : CMakeFiles/qtgame.dir/clean

CMakeFiles/qtgame.dir/depend:
	cd "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build" "/data/data/com.termux/files/home/InnerCode/Command/C&C++/qt/build/CMakeFiles/qtgame.dir/DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/qtgame.dir/depend
