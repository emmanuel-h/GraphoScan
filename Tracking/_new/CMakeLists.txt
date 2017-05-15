# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (graphoScan)

find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_compile_options(-std=c++11)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glut
	GLEW
	pthread
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


add_executable(graphoScan 
	main.cpp 
)
target_link_libraries(graphoScan
	${ALL_LIBS} ${OpenCV_LIBS}
)




add_custom_command(
  TARGET graphoScan POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/graphoScan${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}"
  )



# SOURCE_GROUP(common REGULAR_EXPRESSION ".*/common/.*" )
# SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )


