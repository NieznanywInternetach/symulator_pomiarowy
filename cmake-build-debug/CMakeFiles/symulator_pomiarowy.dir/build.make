# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3.3\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3.3\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\c_folder\symulator_pomiarowy

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\c_folder\symulator_pomiarowy\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/symulator_pomiarowy.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/symulator_pomiarowy.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/symulator_pomiarowy.dir/flags.make

CMakeFiles/symulator_pomiarowy.dir/main.c.obj: CMakeFiles/symulator_pomiarowy.dir/flags.make
CMakeFiles/symulator_pomiarowy.dir/main.c.obj: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\c_folder\symulator_pomiarowy\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/symulator_pomiarowy.dir/main.c.obj"
	C:\c_folder\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\symulator_pomiarowy.dir\main.c.obj -c C:\c_folder\symulator_pomiarowy\main.c

CMakeFiles/symulator_pomiarowy.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/symulator_pomiarowy.dir/main.c.i"
	C:\c_folder\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\c_folder\symulator_pomiarowy\main.c > CMakeFiles\symulator_pomiarowy.dir\main.c.i

CMakeFiles/symulator_pomiarowy.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/symulator_pomiarowy.dir/main.c.s"
	C:\c_folder\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\c_folder\symulator_pomiarowy\main.c -o CMakeFiles\symulator_pomiarowy.dir\main.c.s

# Object files for target symulator_pomiarowy
symulator_pomiarowy_OBJECTS = \
"CMakeFiles/symulator_pomiarowy.dir/main.c.obj"

# External object files for target symulator_pomiarowy
symulator_pomiarowy_EXTERNAL_OBJECTS =

symulator_pomiarowy.exe: CMakeFiles/symulator_pomiarowy.dir/main.c.obj
symulator_pomiarowy.exe: CMakeFiles/symulator_pomiarowy.dir/build.make
symulator_pomiarowy.exe: CMakeFiles/symulator_pomiarowy.dir/linklibs.rsp
symulator_pomiarowy.exe: CMakeFiles/symulator_pomiarowy.dir/objects1.rsp
symulator_pomiarowy.exe: CMakeFiles/symulator_pomiarowy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\c_folder\symulator_pomiarowy\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable symulator_pomiarowy.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\symulator_pomiarowy.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/symulator_pomiarowy.dir/build: symulator_pomiarowy.exe

.PHONY : CMakeFiles/symulator_pomiarowy.dir/build

CMakeFiles/symulator_pomiarowy.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\symulator_pomiarowy.dir\cmake_clean.cmake
.PHONY : CMakeFiles/symulator_pomiarowy.dir/clean

CMakeFiles/symulator_pomiarowy.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\c_folder\symulator_pomiarowy C:\c_folder\symulator_pomiarowy C:\c_folder\symulator_pomiarowy\cmake-build-debug C:\c_folder\symulator_pomiarowy\cmake-build-debug C:\c_folder\symulator_pomiarowy\cmake-build-debug\CMakeFiles\symulator_pomiarowy.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/symulator_pomiarowy.dir/depend

