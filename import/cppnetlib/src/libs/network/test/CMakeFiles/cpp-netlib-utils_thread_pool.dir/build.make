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
include libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/depend.make

# Include the progress variables for this target.
include libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/progress.make

# Include the compile flags for this target's objects.
include libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/flags.make

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/flags.make
libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o: libs/network/test/utils_thread_pool.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/enricohuang/cpp-netlib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o"
	cd /home/enricohuang/cpp-netlib/libs/network/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -Wall -o CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o -c /home/enricohuang/cpp-netlib/libs/network/test/utils_thread_pool.cpp

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.i"
	cd /home/enricohuang/cpp-netlib/libs/network/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -Wall -E /home/enricohuang/cpp-netlib/libs/network/test/utils_thread_pool.cpp > CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.i

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.s"
	cd /home/enricohuang/cpp-netlib/libs/network/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -Wall -S /home/enricohuang/cpp-netlib/libs/network/test/utils_thread_pool.cpp -o CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.s

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.requires:
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.requires

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.provides: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.requires
	$(MAKE) -f libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/build.make libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.provides.build
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.provides

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.provides.build: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o

# Object files for target cpp-netlib-utils_thread_pool
cpp__netlib__utils_thread_pool_OBJECTS = \
"CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o"

# External object files for target cpp-netlib-utils_thread_pool
cpp__netlib__utils_thread_pool_EXTERNAL_OBJECTS =

tests/cpp-netlib-utils_thread_pool: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o
tests/cpp-netlib-utils_thread_pool: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/build.make
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_system.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_regex.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_thread.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libboost_atomic.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libpthread.so
tests/cpp-netlib-utils_thread_pool: libs/network/src/libcppnetlib-uri.a
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libssl.so
tests/cpp-netlib-utils_thread_pool: /usr/lib/x86_64-linux-gnu/libcrypto.so
tests/cpp-netlib-utils_thread_pool: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../tests/cpp-netlib-utils_thread_pool"
	cd /home/enricohuang/cpp-netlib/libs/network/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cpp-netlib-utils_thread_pool.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/build: tests/cpp-netlib-utils_thread_pool
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/build

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/requires: libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/utils_thread_pool.cpp.o.requires
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/requires

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/clean:
	cd /home/enricohuang/cpp-netlib/libs/network/test && $(CMAKE_COMMAND) -P CMakeFiles/cpp-netlib-utils_thread_pool.dir/cmake_clean.cmake
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/clean

libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/depend:
	cd /home/enricohuang/cpp-netlib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/test /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/test /home/enricohuang/cpp-netlib/libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libs/network/test/CMakeFiles/cpp-netlib-utils_thread_pool.dir/depend

