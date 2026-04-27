cmake_minimum_required(VERSION 3.14)
project(MyProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-O3 -ffast-math -g -DNDEBUG)

add_executable(edit_tos 
    src/main.cpp
    src/tree_of_shapes.cpp  
    src/node_tos.cpp
    src/tree_of_shapes_edit.cpp
)

target_include_directories(edit_tos 
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    /home/jmendesforte/Higra/include
)

set(OpenCV_DIR "/usr/lib/x86_64-linux-gnu/cmake/opencv4")
find_package(OpenCV REQUIRED)

target_include_directories(edit_tos PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(edit_tos PRIVATE ${OpenCV_LIBS})