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
include libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/depend.make

# Include the progress variables for this target.
include libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/progress.make

# Include the compile flags for this target's objects.
include libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/flags.make

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/flags.make
libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o: libs/network/test/http/server_constructor_test.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/enricohuang/cpp-netlib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o"
	cd /home/enricohuang/cpp-netlib/libs/network/test/http && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o -c /home/enricohuang/cpp-netlib/libs/network/test/http/server_constructor_test.cpp

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.i"
	cd /home/enricohuang/cpp-netlib/libs/network/test/http && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/enricohuang/cpp-netlib/libs/network/test/http/server_constructor_test.cpp > CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.i

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.s"
	cd /home/enricohuang/cpp-netlib/libs/network/test/http && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/enricohuang/cpp-netlib/libs/network/test/http/server_constructor_test.cpp -o CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.s

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.requires:
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.requires

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.provides: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.requires
	$(MAKE) -f libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/build.make libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.provides.build
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.provides

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.provides.build: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o

# Object files for target cpp-netlib-http-server_constructor_test
cpp__netlib__http__server_constructor_test_OBJECTS = \
"CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o"

# External object files for target cpp-netlib-http-server_constructor_test
cpp__netlib__http__server_constructor_test_EXTERNAL_OBJECTS =

tests/cpp-netlib-http-server_constructor_test: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o
tests/cpp-netlib-http-server_constructor_test: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/build.make
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_system.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_regex.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_thread.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libboost_atomic.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libpthread.so
tests/cpp-netlib-http-server_constructor_test: libs/network/src/libcppnetlib-server-parsers.a
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libssl.so
tests/cpp-netlib-http-server_constructor_test: /usr/lib/x86_64-linux-gnu/libcrypto.so
tests/cpp-netlib-http-server_constructor_test: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../../tests/cpp-netlib-http-server_constructor_test"
	cd /home/enricohuang/cpp-netlib/libs/network/test/http && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cpp-netlib-http-server_constructor_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/build: tests/cpp-netlib-http-server_constructor_test
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/build

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/requires: libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/server_constructor_test.cpp.o.requires
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/requires

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/clean:
	cd /home/enricohuang/cpp-netlib/libs/network/test/http && $(CMAKE_COMMAND) -P CMakeFiles/cpp-netlib-http-server_constructor_test.dir/cmake_clean.cmake
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/clean

libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/depend:
	cd /home/enricohuang/cpp-netlib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/test/http /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/test/http /home/enricohuang/cpp-netlib/libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libs/network/test/http/CMakeFiles/cpp-netlib-http-server_constructor_test.dir/depend

