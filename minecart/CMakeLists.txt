cmake_minimum_required(VERSION 3.16)
project(minecart)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(minecart
    src/main.cpp

    src/Bezier.h
    src/Bezier.cpp

    src/BezierMesh.h
    src/BezierMesh.cpp

    src/ParamCubeCurve.h
    src/ParamCubeCurve.cpp

    src/Camera.h
    src/Camera.cpp

    src/Mesh.h
    src/Mesh.cpp

    src/Cube.h
    src/Cube.cpp

    src/Plane.h
    src/Plane.cpp

    src/FBO.h
    src/FBO.cpp

    src/PostProcessor.h
    src/PostProcessor.cpp

    src/ShaderProgram.h
    src/ShaderProgram.cpp

    external/glad/src/glad.c
)

target_link_libraries(minecart PRIVATE
    glfw
    OpenGL::GL
)

target_include_directories(minecart PRIVATE
    external/glad/include/
    external/glad/src/
    external/stb_image
    shaders/
)