#pragma once

#include <glad/glad.h>
#include "ShaderProgram.h"

class Mesh
{
public:
    unsigned int VAO, VBO;

    virtual ~Mesh();
    virtual void draw(const ShaderProgram& shader) = 0;

protected:
    Mesh() : VAO(0), VBO(0) {}
};