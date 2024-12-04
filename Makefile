# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /workspace

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /workspace

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target test
test:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running tests..."
	/usr/bin/ctest --force-new-ctest-process $(ARGS)
.PHONY : test

# Special rule for the target test
test/fast: test
.PHONY : test/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# Special rule for the target list_install_components
list_install_components:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Available install components are: \"Unspecified\""
.PHONY : list_install_components

# Special rule for the target list_install_components
list_install_components/fast: list_install_components
.PHONY : list_install_components/fast

# Special rule for the target install
install: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install

# Special rule for the target install
install/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Install the project..."
	/usr/bin/cmake -P cmake_install.cmake
.PHONY : install/fast

# Special rule for the target install/local
install/local: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local

# Special rule for the target install/local
install/local/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing only the local directory..."
	/usr/bin/cmake -DCMAKE_INSTALL_LOCAL_ONLY=1 -P cmake_install.cmake
.PHONY : install/local/fast

# Special rule for the target install/strip
install/strip: preinstall
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip

# Special rule for the target install/strip
install/strip/fast: preinstall/fast
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Installing the project stripped..."
	/usr/bin/cmake -DCMAKE_INSTALL_DO_STRIP=1 -P cmake_install.cmake
.PHONY : install/strip/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /workspace/CMakeFiles /workspace//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /workspace/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named webserv_lib

# Build rule for target.
webserv_lib: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 webserv_lib
.PHONY : webserv_lib

# fast build rule for target.
webserv_lib/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/build
.PHONY : webserv_lib/fast

#=============================================================================
# Target rules for targets named webserv_test

# Build rule for target.
webserv_test: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 webserv_test
.PHONY : webserv_test

# fast build rule for target.
webserv_test/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/build
.PHONY : webserv_test/fast

#=============================================================================
# Target rules for targets named gmock

# Build rule for target.
gmock: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gmock
.PHONY : gmock

# fast build rule for target.
gmock/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/build.make _deps/googletest-build/googlemock/CMakeFiles/gmock.dir/build
.PHONY : gmock/fast

#=============================================================================
# Target rules for targets named gmock_main

# Build rule for target.
gmock_main: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gmock_main
.PHONY : gmock_main

# fast build rule for target.
gmock_main/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googlemock/CMakeFiles/gmock_main.dir/build.make _deps/googletest-build/googlemock/CMakeFiles/gmock_main.dir/build
.PHONY : gmock_main/fast

#=============================================================================
# Target rules for targets named gtest

# Build rule for target.
gtest: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gtest
.PHONY : gtest

# fast build rule for target.
gtest/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googletest/CMakeFiles/gtest.dir/build.make _deps/googletest-build/googletest/CMakeFiles/gtest.dir/build
.PHONY : gtest/fast

#=============================================================================
# Target rules for targets named gtest_main

# Build rule for target.
gtest_main: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 gtest_main
.PHONY : gtest_main

# fast build rule for target.
gtest_main/fast:
	$(MAKE) $(MAKESILENT) -f _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/build.make _deps/googletest-build/googletest/CMakeFiles/gtest_main.dir/build
.PHONY : gtest_main/fast

src/configfile/Parser.o: src/configfile/Parser.cpp.o
.PHONY : src/configfile/Parser.o

# target to build an object file
src/configfile/Parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser.cpp.o
.PHONY : src/configfile/Parser.cpp.o

src/configfile/Parser.i: src/configfile/Parser.cpp.i
.PHONY : src/configfile/Parser.i

# target to preprocess a source file
src/configfile/Parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser.cpp.i
.PHONY : src/configfile/Parser.cpp.i

src/configfile/Parser.s: src/configfile/Parser.cpp.s
.PHONY : src/configfile/Parser.s

# target to generate assembly for a file
src/configfile/Parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser.cpp.s
.PHONY : src/configfile/Parser.cpp.s

src/configfile/Parser_helpers.o: src/configfile/Parser_helpers.cpp.o
.PHONY : src/configfile/Parser_helpers.o

# target to build an object file
src/configfile/Parser_helpers.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_helpers.cpp.o
.PHONY : src/configfile/Parser_helpers.cpp.o

src/configfile/Parser_helpers.i: src/configfile/Parser_helpers.cpp.i
.PHONY : src/configfile/Parser_helpers.i

# target to preprocess a source file
src/configfile/Parser_helpers.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_helpers.cpp.i
.PHONY : src/configfile/Parser_helpers.cpp.i

src/configfile/Parser_helpers.s: src/configfile/Parser_helpers.cpp.s
.PHONY : src/configfile/Parser_helpers.s

# target to generate assembly for a file
src/configfile/Parser_helpers.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_helpers.cpp.s
.PHONY : src/configfile/Parser_helpers.cpp.s

src/configfile/Parser_keywords.o: src/configfile/Parser_keywords.cpp.o
.PHONY : src/configfile/Parser_keywords.o

# target to build an object file
src/configfile/Parser_keywords.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_keywords.cpp.o
.PHONY : src/configfile/Parser_keywords.cpp.o

src/configfile/Parser_keywords.i: src/configfile/Parser_keywords.cpp.i
.PHONY : src/configfile/Parser_keywords.i

# target to preprocess a source file
src/configfile/Parser_keywords.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_keywords.cpp.i
.PHONY : src/configfile/Parser_keywords.cpp.i

src/configfile/Parser_keywords.s: src/configfile/Parser_keywords.cpp.s
.PHONY : src/configfile/Parser_keywords.s

# target to generate assembly for a file
src/configfile/Parser_keywords.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/configfile/Parser_keywords.cpp.s
.PHONY : src/configfile/Parser_keywords.cpp.s

src/http/Request.o: src/http/Request.cpp.o
.PHONY : src/http/Request.o

# target to build an object file
src/http/Request.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/http/Request.cpp.o
.PHONY : src/http/Request.cpp.o

src/http/Request.i: src/http/Request.cpp.i
.PHONY : src/http/Request.i

# target to preprocess a source file
src/http/Request.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/http/Request.cpp.i
.PHONY : src/http/Request.cpp.i

src/http/Request.s: src/http/Request.cpp.s
.PHONY : src/http/Request.s

# target to generate assembly for a file
src/http/Request.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/http/Request.cpp.s
.PHONY : src/http/Request.cpp.s

src/logger/ALogger.o: src/logger/ALogger.cpp.o
.PHONY : src/logger/ALogger.o

# target to build an object file
src/logger/ALogger.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ALogger.cpp.o
.PHONY : src/logger/ALogger.cpp.o

src/logger/ALogger.i: src/logger/ALogger.cpp.i
.PHONY : src/logger/ALogger.i

# target to preprocess a source file
src/logger/ALogger.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ALogger.cpp.i
.PHONY : src/logger/ALogger.cpp.i

src/logger/ALogger.s: src/logger/ALogger.cpp.s
.PHONY : src/logger/ALogger.s

# target to generate assembly for a file
src/logger/ALogger.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ALogger.cpp.s
.PHONY : src/logger/ALogger.cpp.s

src/logger/ConsoleLogger.o: src/logger/ConsoleLogger.cpp.o
.PHONY : src/logger/ConsoleLogger.o

# target to build an object file
src/logger/ConsoleLogger.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ConsoleLogger.cpp.o
.PHONY : src/logger/ConsoleLogger.cpp.o

src/logger/ConsoleLogger.i: src/logger/ConsoleLogger.cpp.i
.PHONY : src/logger/ConsoleLogger.i

# target to preprocess a source file
src/logger/ConsoleLogger.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ConsoleLogger.cpp.i
.PHONY : src/logger/ConsoleLogger.cpp.i

src/logger/ConsoleLogger.s: src/logger/ConsoleLogger.cpp.s
.PHONY : src/logger/ConsoleLogger.s

# target to generate assembly for a file
src/logger/ConsoleLogger.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/ConsoleLogger.cpp.s
.PHONY : src/logger/ConsoleLogger.cpp.s

src/logger/FileLogger.o: src/logger/FileLogger.cpp.o
.PHONY : src/logger/FileLogger.o

# target to build an object file
src/logger/FileLogger.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/FileLogger.cpp.o
.PHONY : src/logger/FileLogger.cpp.o

src/logger/FileLogger.i: src/logger/FileLogger.cpp.i
.PHONY : src/logger/FileLogger.i

# target to preprocess a source file
src/logger/FileLogger.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/FileLogger.cpp.i
.PHONY : src/logger/FileLogger.cpp.i

src/logger/FileLogger.s: src/logger/FileLogger.cpp.s
.PHONY : src/logger/FileLogger.s

# target to generate assembly for a file
src/logger/FileLogger.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/FileLogger.cpp.s
.PHONY : src/logger/FileLogger.cpp.s

src/logger/LoggerManager.o: src/logger/LoggerManager.cpp.o
.PHONY : src/logger/LoggerManager.o

# target to build an object file
src/logger/LoggerManager.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/LoggerManager.cpp.o
.PHONY : src/logger/LoggerManager.cpp.o

src/logger/LoggerManager.i: src/logger/LoggerManager.cpp.i
.PHONY : src/logger/LoggerManager.i

# target to preprocess a source file
src/logger/LoggerManager.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/LoggerManager.cpp.i
.PHONY : src/logger/LoggerManager.cpp.i

src/logger/LoggerManager.s: src/logger/LoggerManager.cpp.s
.PHONY : src/logger/LoggerManager.s

# target to generate assembly for a file
src/logger/LoggerManager.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/logger/LoggerManager.cpp.s
.PHONY : src/logger/LoggerManager.cpp.s

src/main.o: src/main.cpp.o
.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/shared/StringUtils.o: src/shared/StringUtils.cpp.o
.PHONY : src/shared/StringUtils.o

# target to build an object file
src/shared/StringUtils.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/shared/StringUtils.cpp.o
.PHONY : src/shared/StringUtils.cpp.o

src/shared/StringUtils.i: src/shared/StringUtils.cpp.i
.PHONY : src/shared/StringUtils.i

# target to preprocess a source file
src/shared/StringUtils.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/shared/StringUtils.cpp.i
.PHONY : src/shared/StringUtils.cpp.i

src/shared/StringUtils.s: src/shared/StringUtils.cpp.s
.PHONY : src/shared/StringUtils.s

# target to generate assembly for a file
src/shared/StringUtils.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/shared/StringUtils.cpp.s
.PHONY : src/shared/StringUtils.cpp.s

src/www/ClientSocket.o: src/www/ClientSocket.cpp.o
.PHONY : src/www/ClientSocket.o

# target to build an object file
src/www/ClientSocket.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ClientSocket.cpp.o
.PHONY : src/www/ClientSocket.cpp.o

src/www/ClientSocket.i: src/www/ClientSocket.cpp.i
.PHONY : src/www/ClientSocket.i

# target to preprocess a source file
src/www/ClientSocket.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ClientSocket.cpp.i
.PHONY : src/www/ClientSocket.cpp.i

src/www/ClientSocket.s: src/www/ClientSocket.cpp.s
.PHONY : src/www/ClientSocket.s

# target to generate assembly for a file
src/www/ClientSocket.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ClientSocket.cpp.s
.PHONY : src/www/ClientSocket.cpp.s

src/www/Connection.o: src/www/Connection.cpp.o
.PHONY : src/www/Connection.o

# target to build an object file
src/www/Connection.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Connection.cpp.o
.PHONY : src/www/Connection.cpp.o

src/www/Connection.i: src/www/Connection.cpp.i
.PHONY : src/www/Connection.i

# target to preprocess a source file
src/www/Connection.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Connection.cpp.i
.PHONY : src/www/Connection.cpp.i

src/www/Connection.s: src/www/Connection.cpp.s
.PHONY : src/www/Connection.s

# target to generate assembly for a file
src/www/Connection.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Connection.cpp.s
.PHONY : src/www/Connection.cpp.s

src/www/CoreServer.o: src/www/CoreServer.cpp.o
.PHONY : src/www/CoreServer.o

# target to build an object file
src/www/CoreServer.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/CoreServer.cpp.o
.PHONY : src/www/CoreServer.cpp.o

src/www/CoreServer.i: src/www/CoreServer.cpp.i
.PHONY : src/www/CoreServer.i

# target to preprocess a source file
src/www/CoreServer.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/CoreServer.cpp.i
.PHONY : src/www/CoreServer.cpp.i

src/www/CoreServer.s: src/www/CoreServer.cpp.s
.PHONY : src/www/CoreServer.s

# target to generate assembly for a file
src/www/CoreServer.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/CoreServer.cpp.s
.PHONY : src/www/CoreServer.cpp.s

src/www/Router.o: src/www/Router.cpp.o
.PHONY : src/www/Router.o

# target to build an object file
src/www/Router.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Router.cpp.o
.PHONY : src/www/Router.cpp.o

src/www/Router.i: src/www/Router.cpp.i
.PHONY : src/www/Router.i

# target to preprocess a source file
src/www/Router.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Router.cpp.i
.PHONY : src/www/Router.cpp.i

src/www/Router.s: src/www/Router.cpp.s
.PHONY : src/www/Router.s

# target to generate assembly for a file
src/www/Router.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/Router.cpp.s
.PHONY : src/www/Router.cpp.s

src/www/ServerSocket.o: src/www/ServerSocket.cpp.o
.PHONY : src/www/ServerSocket.o

# target to build an object file
src/www/ServerSocket.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ServerSocket.cpp.o
.PHONY : src/www/ServerSocket.cpp.o

src/www/ServerSocket.i: src/www/ServerSocket.cpp.i
.PHONY : src/www/ServerSocket.i

# target to preprocess a source file
src/www/ServerSocket.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ServerSocket.cpp.i
.PHONY : src/www/ServerSocket.cpp.i

src/www/ServerSocket.s: src/www/ServerSocket.cpp.s
.PHONY : src/www/ServerSocket.s

# target to generate assembly for a file
src/www/ServerSocket.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/ServerSocket.cpp.s
.PHONY : src/www/ServerSocket.cpp.s

src/www/VirtualServer.o: src/www/VirtualServer.cpp.o
.PHONY : src/www/VirtualServer.o

# target to build an object file
src/www/VirtualServer.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/VirtualServer.cpp.o
.PHONY : src/www/VirtualServer.cpp.o

src/www/VirtualServer.i: src/www/VirtualServer.cpp.i
.PHONY : src/www/VirtualServer.i

# target to preprocess a source file
src/www/VirtualServer.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/VirtualServer.cpp.i
.PHONY : src/www/VirtualServer.cpp.i

src/www/VirtualServer.s: src/www/VirtualServer.cpp.s
.PHONY : src/www/VirtualServer.s

# target to generate assembly for a file
src/www/VirtualServer.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_lib.dir/build.make CMakeFiles/webserv_lib.dir/src/www/VirtualServer.cpp.s
.PHONY : src/www/VirtualServer.cpp.s

tests/configfile/ParserTest.o: tests/configfile/ParserTest.cpp.o
.PHONY : tests/configfile/ParserTest.o

# target to build an object file
tests/configfile/ParserTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/configfile/ParserTest.cpp.o
.PHONY : tests/configfile/ParserTest.cpp.o

tests/configfile/ParserTest.i: tests/configfile/ParserTest.cpp.i
.PHONY : tests/configfile/ParserTest.i

# target to preprocess a source file
tests/configfile/ParserTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/configfile/ParserTest.cpp.i
.PHONY : tests/configfile/ParserTest.cpp.i

tests/configfile/ParserTest.s: tests/configfile/ParserTest.cpp.s
.PHONY : tests/configfile/ParserTest.s

# target to generate assembly for a file
tests/configfile/ParserTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/configfile/ParserTest.cpp.s
.PHONY : tests/configfile/ParserTest.cpp.s

tests/http/RequestTest.o: tests/http/RequestTest.cpp.o
.PHONY : tests/http/RequestTest.o

# target to build an object file
tests/http/RequestTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/http/RequestTest.cpp.o
.PHONY : tests/http/RequestTest.cpp.o

tests/http/RequestTest.i: tests/http/RequestTest.cpp.i
.PHONY : tests/http/RequestTest.i

# target to preprocess a source file
tests/http/RequestTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/http/RequestTest.cpp.i
.PHONY : tests/http/RequestTest.cpp.i

tests/http/RequestTest.s: tests/http/RequestTest.cpp.s
.PHONY : tests/http/RequestTest.s

# target to generate assembly for a file
tests/http/RequestTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/http/RequestTest.cpp.s
.PHONY : tests/http/RequestTest.cpp.s

tests/www/ClientSocketTest.o: tests/www/ClientSocketTest.cpp.o
.PHONY : tests/www/ClientSocketTest.o

# target to build an object file
tests/www/ClientSocketTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ClientSocketTest.cpp.o
.PHONY : tests/www/ClientSocketTest.cpp.o

tests/www/ClientSocketTest.i: tests/www/ClientSocketTest.cpp.i
.PHONY : tests/www/ClientSocketTest.i

# target to preprocess a source file
tests/www/ClientSocketTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ClientSocketTest.cpp.i
.PHONY : tests/www/ClientSocketTest.cpp.i

tests/www/ClientSocketTest.s: tests/www/ClientSocketTest.cpp.s
.PHONY : tests/www/ClientSocketTest.s

# target to generate assembly for a file
tests/www/ClientSocketTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ClientSocketTest.cpp.s
.PHONY : tests/www/ClientSocketTest.cpp.s

tests/www/ConnectionTest.o: tests/www/ConnectionTest.cpp.o
.PHONY : tests/www/ConnectionTest.o

# target to build an object file
tests/www/ConnectionTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ConnectionTest.cpp.o
.PHONY : tests/www/ConnectionTest.cpp.o

tests/www/ConnectionTest.i: tests/www/ConnectionTest.cpp.i
.PHONY : tests/www/ConnectionTest.i

# target to preprocess a source file
tests/www/ConnectionTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ConnectionTest.cpp.i
.PHONY : tests/www/ConnectionTest.cpp.i

tests/www/ConnectionTest.s: tests/www/ConnectionTest.cpp.s
.PHONY : tests/www/ConnectionTest.s

# target to generate assembly for a file
tests/www/ConnectionTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ConnectionTest.cpp.s
.PHONY : tests/www/ConnectionTest.cpp.s

tests/www/CoreServerTest.o: tests/www/CoreServerTest.cpp.o
.PHONY : tests/www/CoreServerTest.o

# target to build an object file
tests/www/CoreServerTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/CoreServerTest.cpp.o
.PHONY : tests/www/CoreServerTest.cpp.o

tests/www/CoreServerTest.i: tests/www/CoreServerTest.cpp.i
.PHONY : tests/www/CoreServerTest.i

# target to preprocess a source file
tests/www/CoreServerTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/CoreServerTest.cpp.i
.PHONY : tests/www/CoreServerTest.cpp.i

tests/www/CoreServerTest.s: tests/www/CoreServerTest.cpp.s
.PHONY : tests/www/CoreServerTest.s

# target to generate assembly for a file
tests/www/CoreServerTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/CoreServerTest.cpp.s
.PHONY : tests/www/CoreServerTest.cpp.s

tests/www/RouterTest.o: tests/www/RouterTest.cpp.o
.PHONY : tests/www/RouterTest.o

# target to build an object file
tests/www/RouterTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/RouterTest.cpp.o
.PHONY : tests/www/RouterTest.cpp.o

tests/www/RouterTest.i: tests/www/RouterTest.cpp.i
.PHONY : tests/www/RouterTest.i

# target to preprocess a source file
tests/www/RouterTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/RouterTest.cpp.i
.PHONY : tests/www/RouterTest.cpp.i

tests/www/RouterTest.s: tests/www/RouterTest.cpp.s
.PHONY : tests/www/RouterTest.s

# target to generate assembly for a file
tests/www/RouterTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/RouterTest.cpp.s
.PHONY : tests/www/RouterTest.cpp.s

tests/www/ServerSocketTest.o: tests/www/ServerSocketTest.cpp.o
.PHONY : tests/www/ServerSocketTest.o

# target to build an object file
tests/www/ServerSocketTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ServerSocketTest.cpp.o
.PHONY : tests/www/ServerSocketTest.cpp.o

tests/www/ServerSocketTest.i: tests/www/ServerSocketTest.cpp.i
.PHONY : tests/www/ServerSocketTest.i

# target to preprocess a source file
tests/www/ServerSocketTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ServerSocketTest.cpp.i
.PHONY : tests/www/ServerSocketTest.cpp.i

tests/www/ServerSocketTest.s: tests/www/ServerSocketTest.cpp.s
.PHONY : tests/www/ServerSocketTest.s

# target to generate assembly for a file
tests/www/ServerSocketTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/ServerSocketTest.cpp.s
.PHONY : tests/www/ServerSocketTest.cpp.s

tests/www/VirtualServerTest.o: tests/www/VirtualServerTest.cpp.o
.PHONY : tests/www/VirtualServerTest.o

# target to build an object file
tests/www/VirtualServerTest.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/VirtualServerTest.cpp.o
.PHONY : tests/www/VirtualServerTest.cpp.o

tests/www/VirtualServerTest.i: tests/www/VirtualServerTest.cpp.i
.PHONY : tests/www/VirtualServerTest.i

# target to preprocess a source file
tests/www/VirtualServerTest.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/VirtualServerTest.cpp.i
.PHONY : tests/www/VirtualServerTest.cpp.i

tests/www/VirtualServerTest.s: tests/www/VirtualServerTest.cpp.s
.PHONY : tests/www/VirtualServerTest.s

# target to generate assembly for a file
tests/www/VirtualServerTest.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/webserv_test.dir/build.make CMakeFiles/webserv_test.dir/tests/www/VirtualServerTest.cpp.s
.PHONY : tests/www/VirtualServerTest.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... install"
	@echo "... install/local"
	@echo "... install/strip"
	@echo "... list_install_components"
	@echo "... rebuild_cache"
	@echo "... test"
	@echo "... gmock"
	@echo "... gmock_main"
	@echo "... gtest"
	@echo "... gtest_main"
	@echo "... webserv_lib"
	@echo "... webserv_test"
	@echo "... src/configfile/Parser.o"
	@echo "... src/configfile/Parser.i"
	@echo "... src/configfile/Parser.s"
	@echo "... src/configfile/Parser_helpers.o"
	@echo "... src/configfile/Parser_helpers.i"
	@echo "... src/configfile/Parser_helpers.s"
	@echo "... src/configfile/Parser_keywords.o"
	@echo "... src/configfile/Parser_keywords.i"
	@echo "... src/configfile/Parser_keywords.s"
	@echo "... src/http/Request.o"
	@echo "... src/http/Request.i"
	@echo "... src/http/Request.s"
	@echo "... src/logger/ALogger.o"
	@echo "... src/logger/ALogger.i"
	@echo "... src/logger/ALogger.s"
	@echo "... src/logger/ConsoleLogger.o"
	@echo "... src/logger/ConsoleLogger.i"
	@echo "... src/logger/ConsoleLogger.s"
	@echo "... src/logger/FileLogger.o"
	@echo "... src/logger/FileLogger.i"
	@echo "... src/logger/FileLogger.s"
	@echo "... src/logger/LoggerManager.o"
	@echo "... src/logger/LoggerManager.i"
	@echo "... src/logger/LoggerManager.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/shared/StringUtils.o"
	@echo "... src/shared/StringUtils.i"
	@echo "... src/shared/StringUtils.s"
	@echo "... src/www/ClientSocket.o"
	@echo "... src/www/ClientSocket.i"
	@echo "... src/www/ClientSocket.s"
	@echo "... src/www/Connection.o"
	@echo "... src/www/Connection.i"
	@echo "... src/www/Connection.s"
	@echo "... src/www/CoreServer.o"
	@echo "... src/www/CoreServer.i"
	@echo "... src/www/CoreServer.s"
	@echo "... src/www/Router.o"
	@echo "... src/www/Router.i"
	@echo "... src/www/Router.s"
	@echo "... src/www/ServerSocket.o"
	@echo "... src/www/ServerSocket.i"
	@echo "... src/www/ServerSocket.s"
	@echo "... src/www/VirtualServer.o"
	@echo "... src/www/VirtualServer.i"
	@echo "... src/www/VirtualServer.s"
	@echo "... tests/configfile/ParserTest.o"
	@echo "... tests/configfile/ParserTest.i"
	@echo "... tests/configfile/ParserTest.s"
	@echo "... tests/http/RequestTest.o"
	@echo "... tests/http/RequestTest.i"
	@echo "... tests/http/RequestTest.s"
	@echo "... tests/www/ClientSocketTest.o"
	@echo "... tests/www/ClientSocketTest.i"
	@echo "... tests/www/ClientSocketTest.s"
	@echo "... tests/www/ConnectionTest.o"
	@echo "... tests/www/ConnectionTest.i"
	@echo "... tests/www/ConnectionTest.s"
	@echo "... tests/www/CoreServerTest.o"
	@echo "... tests/www/CoreServerTest.i"
	@echo "... tests/www/CoreServerTest.s"
	@echo "... tests/www/RouterTest.o"
	@echo "... tests/www/RouterTest.i"
	@echo "... tests/www/RouterTest.s"
	@echo "... tests/www/ServerSocketTest.o"
	@echo "... tests/www/ServerSocketTest.i"
	@echo "... tests/www/ServerSocketTest.s"
	@echo "... tests/www/VirtualServerTest.o"
	@echo "... tests/www/VirtualServerTest.i"
	@echo "... tests/www/VirtualServerTest.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

