cmake_minimum_required(VERSION 2.8)

project(lab9)

add_executable(Seashell.out I_Seashell/Seashell.cpp)
add_executable(Cube.out II_Cube/Cube.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )

target_link_libraries( Seashell.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} )
target_link_libraries( Cube.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} )

