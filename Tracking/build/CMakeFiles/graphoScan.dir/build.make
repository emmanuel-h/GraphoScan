# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/emmanuelh/Git/GraphoScan/Tracking

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/emmanuelh/Git/GraphoScan/Tracking/build

# Include any dependencies generated for this target.
include CMakeFiles/graphoScan.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/graphoScan.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/graphoScan.dir/flags.make

CMakeFiles/graphoScan.dir/HOG.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/HOG.cpp.o: ../HOG.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/graphoScan.dir/HOG.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/HOG.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/HOG.cpp

CMakeFiles/graphoScan.dir/HOG.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/HOG.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/HOG.cpp > CMakeFiles/graphoScan.dir/HOG.cpp.i

CMakeFiles/graphoScan.dir/HOG.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/HOG.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/HOG.cpp -o CMakeFiles/graphoScan.dir/HOG.cpp.s

CMakeFiles/graphoScan.dir/HOG.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/HOG.cpp.o.requires

CMakeFiles/graphoScan.dir/HOG.cpp.o.provides: CMakeFiles/graphoScan.dir/HOG.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/HOG.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/HOG.cpp.o.provides

CMakeFiles/graphoScan.dir/HOG.cpp.o.provides.build: CMakeFiles/graphoScan.dir/HOG.cpp.o


CMakeFiles/graphoScan.dir/Shader.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/Shader.cpp.o: ../Shader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/graphoScan.dir/Shader.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/Shader.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/Shader.cpp

CMakeFiles/graphoScan.dir/Shader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/Shader.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/Shader.cpp > CMakeFiles/graphoScan.dir/Shader.cpp.i

CMakeFiles/graphoScan.dir/Shader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/Shader.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/Shader.cpp -o CMakeFiles/graphoScan.dir/Shader.cpp.s

CMakeFiles/graphoScan.dir/Shader.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/Shader.cpp.o.requires

CMakeFiles/graphoScan.dir/Shader.cpp.o.provides: CMakeFiles/graphoScan.dir/Shader.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/Shader.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/Shader.cpp.o.provides

CMakeFiles/graphoScan.dir/Shader.cpp.o.provides.build: CMakeFiles/graphoScan.dir/Shader.cpp.o


CMakeFiles/graphoScan.dir/Camera.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/Camera.cpp.o: ../Camera.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/graphoScan.dir/Camera.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/Camera.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/Camera.cpp

CMakeFiles/graphoScan.dir/Camera.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/Camera.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/Camera.cpp > CMakeFiles/graphoScan.dir/Camera.cpp.i

CMakeFiles/graphoScan.dir/Camera.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/Camera.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/Camera.cpp -o CMakeFiles/graphoScan.dir/Camera.cpp.s

CMakeFiles/graphoScan.dir/Camera.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/Camera.cpp.o.requires

CMakeFiles/graphoScan.dir/Camera.cpp.o.provides: CMakeFiles/graphoScan.dir/Camera.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/Camera.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/Camera.cpp.o.provides

CMakeFiles/graphoScan.dir/Camera.cpp.o.provides.build: CMakeFiles/graphoScan.dir/Camera.cpp.o


CMakeFiles/graphoScan.dir/GraphoScan.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/GraphoScan.cpp.o: ../GraphoScan.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/graphoScan.dir/GraphoScan.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/GraphoScan.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/GraphoScan.cpp

CMakeFiles/graphoScan.dir/GraphoScan.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/GraphoScan.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/GraphoScan.cpp > CMakeFiles/graphoScan.dir/GraphoScan.cpp.i

CMakeFiles/graphoScan.dir/GraphoScan.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/GraphoScan.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/GraphoScan.cpp -o CMakeFiles/graphoScan.dir/GraphoScan.cpp.s

CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.requires

CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.provides: CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.provides

CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.provides.build: CMakeFiles/graphoScan.dir/GraphoScan.cpp.o


CMakeFiles/graphoScan.dir/OpenGL.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/OpenGL.cpp.o: ../OpenGL.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/graphoScan.dir/OpenGL.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/OpenGL.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/OpenGL.cpp

CMakeFiles/graphoScan.dir/OpenGL.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/OpenGL.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/OpenGL.cpp > CMakeFiles/graphoScan.dir/OpenGL.cpp.i

CMakeFiles/graphoScan.dir/OpenGL.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/OpenGL.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/OpenGL.cpp -o CMakeFiles/graphoScan.dir/OpenGL.cpp.s

CMakeFiles/graphoScan.dir/OpenGL.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/OpenGL.cpp.o.requires

CMakeFiles/graphoScan.dir/OpenGL.cpp.o.provides: CMakeFiles/graphoScan.dir/OpenGL.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/OpenGL.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/OpenGL.cpp.o.provides

CMakeFiles/graphoScan.dir/OpenGL.cpp.o.provides.build: CMakeFiles/graphoScan.dir/OpenGL.cpp.o


CMakeFiles/graphoScan.dir/main.cpp.o: CMakeFiles/graphoScan.dir/flags.make
CMakeFiles/graphoScan.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/graphoScan.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/graphoScan.dir/main.cpp.o -c /home/emmanuelh/Git/GraphoScan/Tracking/main.cpp

CMakeFiles/graphoScan.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/graphoScan.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/emmanuelh/Git/GraphoScan/Tracking/main.cpp > CMakeFiles/graphoScan.dir/main.cpp.i

CMakeFiles/graphoScan.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/graphoScan.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/emmanuelh/Git/GraphoScan/Tracking/main.cpp -o CMakeFiles/graphoScan.dir/main.cpp.s

CMakeFiles/graphoScan.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/graphoScan.dir/main.cpp.o.requires

CMakeFiles/graphoScan.dir/main.cpp.o.provides: CMakeFiles/graphoScan.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/graphoScan.dir/build.make CMakeFiles/graphoScan.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/graphoScan.dir/main.cpp.o.provides

CMakeFiles/graphoScan.dir/main.cpp.o.provides.build: CMakeFiles/graphoScan.dir/main.cpp.o


# Object files for target graphoScan
graphoScan_OBJECTS = \
"CMakeFiles/graphoScan.dir/HOG.cpp.o" \
"CMakeFiles/graphoScan.dir/Shader.cpp.o" \
"CMakeFiles/graphoScan.dir/Camera.cpp.o" \
"CMakeFiles/graphoScan.dir/GraphoScan.cpp.o" \
"CMakeFiles/graphoScan.dir/OpenGL.cpp.o" \
"CMakeFiles/graphoScan.dir/main.cpp.o"

# External object files for target graphoScan
graphoScan_EXTERNAL_OBJECTS =

graphoScan: CMakeFiles/graphoScan.dir/HOG.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/Shader.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/Camera.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/GraphoScan.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/OpenGL.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/main.cpp.o
graphoScan: CMakeFiles/graphoScan.dir/build.make
graphoScan: /usr/lib/x86_64-linux-gnu/libGL.so
graphoScan: /usr/local/lib/libopencv_cudabgsegm.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudaobjdetect.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudastereo.so.3.2.0
graphoScan: /usr/local/lib/libopencv_stitching.so.3.2.0
graphoScan: /usr/local/lib/libopencv_superres.so.3.2.0
graphoScan: /usr/local/lib/libopencv_videostab.so.3.2.0
graphoScan: /usr/local/lib/libopencv_aruco.so.3.2.0
graphoScan: /usr/local/lib/libopencv_bgsegm.so.3.2.0
graphoScan: /usr/local/lib/libopencv_bioinspired.so.3.2.0
graphoScan: /usr/local/lib/libopencv_ccalib.so.3.2.0
graphoScan: /usr/local/lib/libopencv_dpm.so.3.2.0
graphoScan: /usr/local/lib/libopencv_face.so.3.2.0
graphoScan: /usr/local/lib/libopencv_freetype.so.3.2.0
graphoScan: /usr/local/lib/libopencv_fuzzy.so.3.2.0
graphoScan: /usr/local/lib/libopencv_line_descriptor.so.3.2.0
graphoScan: /usr/local/lib/libopencv_optflow.so.3.2.0
graphoScan: /usr/local/lib/libopencv_reg.so.3.2.0
graphoScan: /usr/local/lib/libopencv_rgbd.so.3.2.0
graphoScan: /usr/local/lib/libopencv_saliency.so.3.2.0
graphoScan: /usr/local/lib/libopencv_stereo.so.3.2.0
graphoScan: /usr/local/lib/libopencv_structured_light.so.3.2.0
graphoScan: /usr/local/lib/libopencv_surface_matching.so.3.2.0
graphoScan: /usr/local/lib/libopencv_tracking.so.3.2.0
graphoScan: /usr/local/lib/libopencv_xfeatures2d.so.3.2.0
graphoScan: /usr/local/lib/libopencv_ximgproc.so.3.2.0
graphoScan: /usr/local/lib/libopencv_xobjdetect.so.3.2.0
graphoScan: /usr/local/lib/libopencv_xphoto.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudafeatures2d.so.3.2.0
graphoScan: /usr/local/lib/libopencv_shape.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudacodec.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudaoptflow.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudalegacy.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudawarping.so.3.2.0
graphoScan: /usr/local/lib/libopencv_photo.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudaimgproc.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudafilters.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudaarithm.so.3.2.0
graphoScan: /usr/local/lib/libopencv_calib3d.so.3.2.0
graphoScan: /usr/local/lib/libopencv_phase_unwrapping.so.3.2.0
graphoScan: /usr/local/lib/libopencv_video.so.3.2.0
graphoScan: /usr/local/lib/libopencv_datasets.so.3.2.0
graphoScan: /usr/local/lib/libopencv_dnn.so.3.2.0
graphoScan: /usr/local/lib/libopencv_plot.so.3.2.0
graphoScan: /usr/local/lib/libopencv_text.so.3.2.0
graphoScan: /usr/local/lib/libopencv_features2d.so.3.2.0
graphoScan: /usr/local/lib/libopencv_flann.so.3.2.0
graphoScan: /usr/local/lib/libopencv_highgui.so.3.2.0
graphoScan: /usr/local/lib/libopencv_ml.so.3.2.0
graphoScan: /usr/local/lib/libopencv_videoio.so.3.2.0
graphoScan: /usr/local/lib/libopencv_imgcodecs.so.3.2.0
graphoScan: /usr/local/lib/libopencv_objdetect.so.3.2.0
graphoScan: /usr/local/lib/libopencv_imgproc.so.3.2.0
graphoScan: /usr/local/lib/libopencv_core.so.3.2.0
graphoScan: /usr/local/lib/libopencv_cudev.so.3.2.0
graphoScan: CMakeFiles/graphoScan.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable graphoScan"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/graphoScan.dir/link.txt --verbose=$(VERBOSE)
	/usr/bin/cmake -E copy /home/emmanuelh/Git/GraphoScan/Tracking/build/./graphoScan /home/emmanuelh/Git/GraphoScan/Tracking

# Rule to build all files generated by this target.
CMakeFiles/graphoScan.dir/build: graphoScan

.PHONY : CMakeFiles/graphoScan.dir/build

CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/HOG.cpp.o.requires
CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/Shader.cpp.o.requires
CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/Camera.cpp.o.requires
CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/GraphoScan.cpp.o.requires
CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/OpenGL.cpp.o.requires
CMakeFiles/graphoScan.dir/requires: CMakeFiles/graphoScan.dir/main.cpp.o.requires

.PHONY : CMakeFiles/graphoScan.dir/requires

CMakeFiles/graphoScan.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/graphoScan.dir/cmake_clean.cmake
.PHONY : CMakeFiles/graphoScan.dir/clean

CMakeFiles/graphoScan.dir/depend:
	cd /home/emmanuelh/Git/GraphoScan/Tracking/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/emmanuelh/Git/GraphoScan/Tracking /home/emmanuelh/Git/GraphoScan/Tracking /home/emmanuelh/Git/GraphoScan/Tracking/build /home/emmanuelh/Git/GraphoScan/Tracking/build /home/emmanuelh/Git/GraphoScan/Tracking/build/CMakeFiles/graphoScan.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/graphoScan.dir/depend

