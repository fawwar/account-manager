# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/enricohuang/cpp-netlib

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/enricohuang/cpp-netlib

# Include any dependencies generated for this target.
include libs/network/example/CMakeFiles/trivial_google.dir/depend.make

# Include the progress variables for this target.
include libs/network/example/CMakeFiles/trivial_google.dir/progress.make

# Include the compile flags for this target's objects.
include libs/network/example/CMakeFiles/trivial_google.dir/flags.make

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o: libs/network/example/CMakeFiles/trivial_google.dir/flags.make
libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o: libs/network/example/trivial_google.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/enricohuang/cpp-netlib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/trivial_google.dir/trivial_google.cpp.o -c /home/enricohuang/cpp-netlib/libs/network/example/trivial_google.cpp

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/trivial_google.dir/trivial_google.cpp.i"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/enricohuang/cpp-netlib/libs/network/example/trivial_google.cpp > CMakeFiles/trivial_google.dir/trivial_google.cpp.i

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/trivial_google.dir/trivial_google.cpp.s"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/enricohuang/cpp-netlib/libs/network/example/trivial_google.cpp -o CMakeFiles/trivial_google.dir/trivial_google.cpp.s

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.requires:
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.requires

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.provides: libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.requires
	$(MAKE) -f libs/network/example/CMakeFiles/trivial_google.dir/build.make libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.provides.build
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.provides

libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.provides.build: libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o

# Object files for target trivial_google
trivial_google_OBJECTS = \
"CMakeFiles/trivial_google.dir/trivial_google.cpp.o"

# External object files for target trivial_google
trivial_google_EXTERNAL_OBJECTS =

example/trivial_google: libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o
example/trivial_google: libs/network/example/CMakeFiles/trivial_google.dir/build.make
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_thread.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_regex.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_system.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
example/trivial_google: libs/network/src/libcppnetlib-uri.a
example/trivial_google: libs/network/src/libcppnetlib-client-connections.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libssl.so
example/trivial_google: /usr/lib/x86_64-linux-gnu/libcrypto.so
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_thread.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_regex.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_system.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libboost_atomic.a
example/trivial_google: /usr/lib/x86_64-linux-gnu/libpthread.so
example/trivial_google: libs/network/example/CMakeFiles/trivial_google.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../example/trivial_google"
	cd /home/enricohuang/cpp-netlib/libs/network/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/trivial_google.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/network/example/CMakeFiles/trivial_google.dir/build: example/trivial_google
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/build

libs/network/example/CMakeFiles/trivial_google.dir/requires: libs/network/example/CMakeFiles/trivial_google.dir/trivial_google.cpp.o.requires
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/requires

libs/network/example/CMakeFiles/trivial_google.dir/clean:
	cd /home/enricohuang/cpp-netlib/libs/network/example && $(CMAKE_COMMAND) -P CMakeFiles/trivial_google.dir/cmake_clean.cmake
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/clean

libs/network/example/CMakeFiles/trivial_google.dir/depend:
	cd /home/enricohuang/cpp-netlib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/example /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/example /home/enricohuang/cpp-netlib/libs/network/example/CMakeFiles/trivial_google.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libs/network/example/CMakeFiles/trivial_google.dir/depend

