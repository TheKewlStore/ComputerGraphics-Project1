cmake_minimum_required(VERSION 3.9)
project(Project1)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread -fpermissive")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(Project1 main.cpp)

target_link_libraries(Project1 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
