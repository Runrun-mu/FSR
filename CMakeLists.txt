cmake_minimum_required(VERSION 3.11)
project(fsr)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(glfw3 REQUIRED)

include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS})
include_directories( ${GLEW_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/external)

add_executable(fsr glad.c main.cpp Progaram.hpp Progaram.cpp Shader.hpp Shader.cpp ResourceManager.hpp ResourceManager.cpp Utils.hpp Utils.cpp) 

target_link_libraries(fsr ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} glfw dl)