# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /home/gbowne1/Documents/88os

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gbowne1/Documents/88os/build

# Include any dependencies generated for this target.
include CMakeFiles/kernel-x86_64.bin.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/kernel-x86_64.bin.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kernel-x86_64.bin.dir/flags.make

CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o: CMakeFiles/kernel-x86_64.bin.dir/flags.make
CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o: ../install.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gbowne1/Documents/88os/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o"
	/bin/clang++-11 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o -c /home/gbowne1/Documents/88os/install.cpp

CMakeFiles/kernel-x86_64.bin.dir/install.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kernel-x86_64.bin.dir/install.cpp.i"
	/bin/clang++-11 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gbowne1/Documents/88os/install.cpp > CMakeFiles/kernel-x86_64.bin.dir/install.cpp.i

CMakeFiles/kernel-x86_64.bin.dir/install.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kernel-x86_64.bin.dir/install.cpp.s"
	/bin/clang++-11 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gbowne1/Documents/88os/install.cpp -o CMakeFiles/kernel-x86_64.bin.dir/install.cpp.s

# Object files for target kernel-x86_64.bin
kernel__x86_64_bin_OBJECTS = \
"CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o"

# External object files for target kernel-x86_64.bin
kernel__x86_64_bin_EXTERNAL_OBJECTS =

kernel-x86_64.bin: CMakeFiles/kernel-x86_64.bin.dir/install.cpp.o
kernel-x86_64.bin: CMakeFiles/kernel-x86_64.bin.dir/build.make
kernel-x86_64.bin: CMakeFiles/kernel-x86_64.bin.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gbowne1/Documents/88os/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable kernel-x86_64.bin"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kernel-x86_64.bin.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kernel-x86_64.bin.dir/build: kernel-x86_64.bin

.PHONY : CMakeFiles/kernel-x86_64.bin.dir/build

CMakeFiles/kernel-x86_64.bin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kernel-x86_64.bin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kernel-x86_64.bin.dir/clean

CMakeFiles/kernel-x86_64.bin.dir/depend:
	cd /home/gbowne1/Documents/88os/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gbowne1/Documents/88os /home/gbowne1/Documents/88os /home/gbowne1/Documents/88os/build /home/gbowne1/Documents/88os/build /home/gbowne1/Documents/88os/build/CMakeFiles/kernel-x86_64.bin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/kernel-x86_64.bin.dir/depend

