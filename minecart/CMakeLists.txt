cmake_minimum_required(VERSION 3.16)
project(minecart)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_executable(minecart
    src/main.cpp

    src/Bezier.cpp
    src/ParamCubeCurve.cpp

    src/Camera.cpp
    src/Room.cpp
    
    src/Mesh.cpp
    src/Model.cpp
    src/Shader.cpp

    src/RailPlacer.cpp
    src/Animating.cpp

    external/glad/src/glad.c
 )

target_link_libraries(minecart PRIVATE
    glfw
    OpenGL::GL
    assimp::assimp
)

target_include_directories(minecart PRIVATE
    external/glad/include
    external/stb_image
    shaders/
)