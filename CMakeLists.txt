cmake_minimum_required(VERSION 3.9)
project(Project1)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -fpermissive")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})

add_executable(Project1 src/main.cpp src/plyfile.cpp)

target_link_libraries(Project1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
