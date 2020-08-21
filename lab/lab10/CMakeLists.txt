cmake_minimum_required(VERSION 2.8)

project(lab10)

add_executable(CubePatches.out I_CubePatches/CubePatches.cpp)
add_executable(TeapotPatches.out II_TeapotPatches/TeapotPatches.cpp)
add_executable(TerrainPatches.out III_TerrainPatches/TerrainPatches.cpp)


find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} )


target_link_libraries( CubePatches.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} )
target_link_libraries( TeapotPatches.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} )
target_link_libraries( TerrainPatches.out ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARIES} )
