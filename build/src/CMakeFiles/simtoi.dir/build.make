# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_SOURCE_DIR = /home/bkloppenborg/workspace/simtoi

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bkloppenborg/workspace/simtoi/build

# Include any dependencies generated for this target.
include src/CMakeFiles/simtoi.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/simtoi.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/simtoi.dir/flags.make

src/CMakeFiles/simtoi.dir/CModelList.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CModelList.cpp.o: ../src/CModelList.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CModelList.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CModelList.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CModelList.cpp

src/CMakeFiles/simtoi.dir/CModelList.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CModelList.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CModelList.cpp > CMakeFiles/simtoi.dir/CModelList.cpp.i

src/CMakeFiles/simtoi.dir/CModelList.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CModelList.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CModelList.cpp -o CMakeFiles/simtoi.dir/CModelList.cpp.s

src/CMakeFiles/simtoi.dir/CModelList.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CModelList.cpp.o.requires

src/CMakeFiles/simtoi.dir/CModelList.cpp.o.provides: src/CMakeFiles/simtoi.dir/CModelList.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CModelList.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CModelList.cpp.o.provides

src/CMakeFiles/simtoi.dir/CModelList.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CModelList.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CModelList.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o: ../src/CPositionXY.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CPositionXY.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CPositionXY.cpp

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CPositionXY.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CPositionXY.cpp > CMakeFiles/simtoi.dir/CPositionXY.cpp.i

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CPositionXY.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CPositionXY.cpp -o CMakeFiles/simtoi.dir/CPositionXY.cpp.s

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.requires

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.provides: src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.provides

src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/CPosition.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CPosition.cpp.o: ../src/CPosition.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CPosition.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CPosition.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CPosition.cpp

src/CMakeFiles/simtoi.dir/CPosition.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CPosition.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CPosition.cpp > CMakeFiles/simtoi.dir/CPosition.cpp.i

src/CMakeFiles/simtoi.dir/CPosition.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CPosition.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CPosition.cpp -o CMakeFiles/simtoi.dir/CPosition.cpp.s

src/CMakeFiles/simtoi.dir/CPosition.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CPosition.cpp.o.requires

src/CMakeFiles/simtoi.dir/CPosition.cpp.o.provides: src/CMakeFiles/simtoi.dir/CPosition.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CPosition.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CPosition.cpp.o.provides

src/CMakeFiles/simtoi.dir/CPosition.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CPosition.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CPosition.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/CModel.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CModel.cpp.o: ../src/CModel.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CModel.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CModel.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CModel.cpp

src/CMakeFiles/simtoi.dir/CModel.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CModel.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CModel.cpp > CMakeFiles/simtoi.dir/CModel.cpp.i

src/CMakeFiles/simtoi.dir/CModel.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CModel.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CModel.cpp -o CMakeFiles/simtoi.dir/CModel.cpp.s

src/CMakeFiles/simtoi.dir/CModel.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CModel.cpp.o.requires

src/CMakeFiles/simtoi.dir/CModel.cpp.o.provides: src/CMakeFiles/simtoi.dir/CModel.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CModel.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CModel.cpp.o.provides

src/CMakeFiles/simtoi.dir/CModel.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CModel.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CModel.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/CFeature.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CFeature.cpp.o: ../src/CFeature.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CFeature.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CFeature.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CFeature.cpp

src/CMakeFiles/simtoi.dir/CFeature.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CFeature.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CFeature.cpp > CMakeFiles/simtoi.dir/CFeature.cpp.i

src/CMakeFiles/simtoi.dir/CFeature.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CFeature.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CFeature.cpp -o CMakeFiles/simtoi.dir/CFeature.cpp.s

src/CMakeFiles/simtoi.dir/CFeature.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CFeature.cpp.o.requires

src/CMakeFiles/simtoi.dir/CFeature.cpp.o.provides: src/CMakeFiles/simtoi.dir/CFeature.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CFeature.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CFeature.cpp.o.provides

src/CMakeFiles/simtoi.dir/CFeature.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CFeature.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CFeature.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/main.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/main.cpp.o: ../src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/main.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/main.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/main.cpp

src/CMakeFiles/simtoi.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/main.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/main.cpp > CMakeFiles/simtoi.dir/main.cpp.i

src/CMakeFiles/simtoi.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/main.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/main.cpp -o CMakeFiles/simtoi.dir/main.cpp.s

src/CMakeFiles/simtoi.dir/main.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/main.cpp.o.requires

src/CMakeFiles/simtoi.dir/main.cpp.o.provides: src/CMakeFiles/simtoi.dir/main.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/main.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/main.cpp.o.provides

src/CMakeFiles/simtoi.dir/main.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/main.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/main.cpp.o.provides.build

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o: src/CMakeFiles/simtoi.dir/flags.make
src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o: ../src/CModelSphere.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/bkloppenborg/workspace/simtoi/build/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/simtoi.dir/CModelSphere.cpp.o -c /home/bkloppenborg/workspace/simtoi/src/CModelSphere.cpp

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/simtoi.dir/CModelSphere.cpp.i"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/bkloppenborg/workspace/simtoi/src/CModelSphere.cpp > CMakeFiles/simtoi.dir/CModelSphere.cpp.i

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/simtoi.dir/CModelSphere.cpp.s"
	cd /home/bkloppenborg/workspace/simtoi/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/bkloppenborg/workspace/simtoi/src/CModelSphere.cpp -o CMakeFiles/simtoi.dir/CModelSphere.cpp.s

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.requires:
.PHONY : src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.requires

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.provides: src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/simtoi.dir/build.make src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.provides.build
.PHONY : src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.provides

src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.provides.build: src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o
.PHONY : src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.provides.build

# Object files for target simtoi
simtoi_OBJECTS = \
"CMakeFiles/simtoi.dir/CModelList.cpp.o" \
"CMakeFiles/simtoi.dir/CPositionXY.cpp.o" \
"CMakeFiles/simtoi.dir/CPosition.cpp.o" \
"CMakeFiles/simtoi.dir/CModel.cpp.o" \
"CMakeFiles/simtoi.dir/CFeature.cpp.o" \
"CMakeFiles/simtoi.dir/main.cpp.o" \
"CMakeFiles/simtoi.dir/CModelSphere.cpp.o"

# External object files for target simtoi
simtoi_EXTERNAL_OBJECTS =

../bin/simtoi: src/CMakeFiles/simtoi.dir/CModelList.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/CPosition.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/CModel.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/CFeature.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/main.cpp.o
../bin/simtoi: src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o
../bin/simtoi: /usr/lib/libglut.so
../bin/simtoi: /usr/lib/libXmu.so
../bin/simtoi: /usr/lib/libXi.so
../bin/simtoi: /usr/lib/libGLU.so
../bin/simtoi: /usr/lib/libGL.so
../bin/simtoi: /usr/lib/libSM.so
../bin/simtoi: /usr/lib/libICE.so
../bin/simtoi: /usr/lib/libX11.so
../bin/simtoi: /usr/lib/libXext.so
../bin/simtoi: src/CMakeFiles/simtoi.dir/build.make
../bin/simtoi: src/CMakeFiles/simtoi.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../bin/simtoi"
	cd /home/bkloppenborg/workspace/simtoi/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simtoi.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/simtoi.dir/build: ../bin/simtoi
.PHONY : src/CMakeFiles/simtoi.dir/build

src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CModelList.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CPositionXY.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CPosition.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CModel.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CFeature.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/main.cpp.o.requires
src/CMakeFiles/simtoi.dir/requires: src/CMakeFiles/simtoi.dir/CModelSphere.cpp.o.requires
.PHONY : src/CMakeFiles/simtoi.dir/requires

src/CMakeFiles/simtoi.dir/clean:
	cd /home/bkloppenborg/workspace/simtoi/build/src && $(CMAKE_COMMAND) -P CMakeFiles/simtoi.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/simtoi.dir/clean

src/CMakeFiles/simtoi.dir/depend:
	cd /home/bkloppenborg/workspace/simtoi/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bkloppenborg/workspace/simtoi /home/bkloppenborg/workspace/simtoi/src /home/bkloppenborg/workspace/simtoi/build /home/bkloppenborg/workspace/simtoi/build/src /home/bkloppenborg/workspace/simtoi/build/src/CMakeFiles/simtoi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/simtoi.dir/depend

