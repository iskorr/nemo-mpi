# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/isk3nd3r/NeMo/project/nemo-mpi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/isk3nd3r/NeMo/project/nemo-mpi/build

# Include any dependencies generated for this target.
include src/examples/CMakeFiles/torus.dir/depend.make

# Include the progress variables for this target.
include src/examples/CMakeFiles/torus.dir/progress.make

# Include the compile flags for this target's objects.
include src/examples/CMakeFiles/torus.dir/flags.make

src/examples/CMakeFiles/torus.dir/torus.cpp.o: src/examples/CMakeFiles/torus.dir/flags.make
src/examples/CMakeFiles/torus.dir/torus.cpp.o: ../src/examples/torus.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/isk3nd3r/NeMo/project/nemo-mpi/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/examples/CMakeFiles/torus.dir/torus.cpp.o"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/torus.dir/torus.cpp.o -c /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/torus.cpp

src/examples/CMakeFiles/torus.dir/torus.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/torus.dir/torus.cpp.i"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/torus.cpp > CMakeFiles/torus.dir/torus.cpp.i

src/examples/CMakeFiles/torus.dir/torus.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/torus.dir/torus.cpp.s"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/torus.cpp -o CMakeFiles/torus.dir/torus.cpp.s

src/examples/CMakeFiles/torus.dir/torus.cpp.o.requires:
.PHONY : src/examples/CMakeFiles/torus.dir/torus.cpp.o.requires

src/examples/CMakeFiles/torus.dir/torus.cpp.o.provides: src/examples/CMakeFiles/torus.dir/torus.cpp.o.requires
	$(MAKE) -f src/examples/CMakeFiles/torus.dir/build.make src/examples/CMakeFiles/torus.dir/torus.cpp.o.provides.build
.PHONY : src/examples/CMakeFiles/torus.dir/torus.cpp.o.provides

src/examples/CMakeFiles/torus.dir/torus.cpp.o.provides.build: src/examples/CMakeFiles/torus.dir/torus.cpp.o

src/examples/CMakeFiles/torus.dir/common.cpp.o: src/examples/CMakeFiles/torus.dir/flags.make
src/examples/CMakeFiles/torus.dir/common.cpp.o: ../src/examples/common.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/isk3nd3r/NeMo/project/nemo-mpi/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/examples/CMakeFiles/torus.dir/common.cpp.o"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/torus.dir/common.cpp.o -c /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/common.cpp

src/examples/CMakeFiles/torus.dir/common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/torus.dir/common.cpp.i"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/common.cpp > CMakeFiles/torus.dir/common.cpp.i

src/examples/CMakeFiles/torus.dir/common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/torus.dir/common.cpp.s"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples/common.cpp -o CMakeFiles/torus.dir/common.cpp.s

src/examples/CMakeFiles/torus.dir/common.cpp.o.requires:
.PHONY : src/examples/CMakeFiles/torus.dir/common.cpp.o.requires

src/examples/CMakeFiles/torus.dir/common.cpp.o.provides: src/examples/CMakeFiles/torus.dir/common.cpp.o.requires
	$(MAKE) -f src/examples/CMakeFiles/torus.dir/build.make src/examples/CMakeFiles/torus.dir/common.cpp.o.provides.build
.PHONY : src/examples/CMakeFiles/torus.dir/common.cpp.o.provides

src/examples/CMakeFiles/torus.dir/common.cpp.o.provides.build: src/examples/CMakeFiles/torus.dir/common.cpp.o

# Object files for target torus
torus_OBJECTS = \
"CMakeFiles/torus.dir/torus.cpp.o" \
"CMakeFiles/torus.dir/common.cpp.o"

# External object files for target torus
torus_EXTERNAL_OBJECTS =

src/examples/torus: src/examples/CMakeFiles/torus.dir/torus.cpp.o
src/examples/torus: src/examples/CMakeFiles/torus.dir/common.cpp.o
src/examples/torus: src/nemo/libnemo.so
src/examples/torus: /usr/lib/libboost_date_time.so
src/examples/torus: /usr/lib/libboost_filesystem.so
src/examples/torus: /usr/lib/libboost_program_options.so
src/examples/torus: /usr/lib/libboost_system.so
src/examples/torus: /usr/lib/libboost_program_options.so
src/examples/torus: src/nemo/cpu/libnemo_cpu.so
src/examples/torus: src/nemo/libnemo_base.so
src/examples/torus: /usr/lib/x86_64-linux-gnu/libltdl.so
src/examples/torus: /usr/lib/libboost_date_time.so
src/examples/torus: /usr/lib/libboost_filesystem.so
src/examples/torus: /usr/lib/libboost_program_options.so
src/examples/torus: /usr/lib/libboost_system.so
src/examples/torus: src/examples/CMakeFiles/torus.dir/build.make
src/examples/torus: src/examples/CMakeFiles/torus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable torus"
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/torus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/examples/CMakeFiles/torus.dir/build: src/examples/torus
.PHONY : src/examples/CMakeFiles/torus.dir/build

src/examples/CMakeFiles/torus.dir/requires: src/examples/CMakeFiles/torus.dir/torus.cpp.o.requires
src/examples/CMakeFiles/torus.dir/requires: src/examples/CMakeFiles/torus.dir/common.cpp.o.requires
.PHONY : src/examples/CMakeFiles/torus.dir/requires

src/examples/CMakeFiles/torus.dir/clean:
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples && $(CMAKE_COMMAND) -P CMakeFiles/torus.dir/cmake_clean.cmake
.PHONY : src/examples/CMakeFiles/torus.dir/clean

src/examples/CMakeFiles/torus.dir/depend:
	cd /home/isk3nd3r/NeMo/project/nemo-mpi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/isk3nd3r/NeMo/project/nemo-mpi /home/isk3nd3r/NeMo/project/nemo-mpi/src/examples /home/isk3nd3r/NeMo/project/nemo-mpi/build /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples /home/isk3nd3r/NeMo/project/nemo-mpi/build/src/examples/CMakeFiles/torus.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/examples/CMakeFiles/torus.dir/depend

