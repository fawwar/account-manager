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
include libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/depend.make

# Include the progress variables for this target.
include libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/progress.make

# Include the compile flags for this target's objects.
include libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/flags.make

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/flags.make
libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o: libs/network/example/http/hello_world_async_server_with_work_queue.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/enricohuang/cpp-netlib/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o -c /home/enricohuang/cpp-netlib/libs/network/example/http/hello_world_async_server_with_work_queue.cpp

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.i"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/enricohuang/cpp-netlib/libs/network/example/http/hello_world_async_server_with_work_queue.cpp > CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.i

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.s"
	cd /home/enricohuang/cpp-netlib/libs/network/example && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/enricohuang/cpp-netlib/libs/network/example/http/hello_world_async_server_with_work_queue.cpp -o CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.s

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.requires:
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.requires

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.provides: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.requires
	$(MAKE) -f libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/build.make libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.provides.build
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.provides

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.provides.build: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o

# Object files for target hello_world_async_server_with_work_queue
hello_world_async_server_with_work_queue_OBJECTS = \
"CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o"

# External object files for target hello_world_async_server_with_work_queue
hello_world_async_server_with_work_queue_EXTERNAL_OBJECTS =

example/hello_world_async_server_with_work_queue: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o
example/hello_world_async_server_with_work_queue: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/build.make
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_thread.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_regex.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_system.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
example/hello_world_async_server_with_work_queue: libs/network/src/libcppnetlib-uri.a
example/hello_world_async_server_with_work_queue: libs/network/src/libcppnetlib-client-connections.a
example/hello_world_async_server_with_work_queue: libs/network/src/libcppnetlib-server-parsers.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libssl.so
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libcrypto.so
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_unit_test_framework.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_thread.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_date_time.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_regex.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_system.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_chrono.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libboost_atomic.a
example/hello_world_async_server_with_work_queue: /usr/lib/x86_64-linux-gnu/libpthread.so
example/hello_world_async_server_with_work_queue: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../../example/hello_world_async_server_with_work_queue"
	cd /home/enricohuang/cpp-netlib/libs/network/example && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hello_world_async_server_with_work_queue.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/build: example/hello_world_async_server_with_work_queue
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/build

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/requires: libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/http/hello_world_async_server_with_work_queue.cpp.o.requires
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/requires

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/clean:
	cd /home/enricohuang/cpp-netlib/libs/network/example && $(CMAKE_COMMAND) -P CMakeFiles/hello_world_async_server_with_work_queue.dir/cmake_clean.cmake
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/clean

libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/depend:
	cd /home/enricohuang/cpp-netlib && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/example /home/enricohuang/cpp-netlib /home/enricohuang/cpp-netlib/libs/network/example /home/enricohuang/cpp-netlib/libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libs/network/example/CMakeFiles/hello_world_async_server_with_work_queue.dir/depend

