#pragma once

#include "Mesh.h"
#include <glm/glm.hpp>

unsigned int loadTexture(const char* path);

class Cube : public Mesh
{
public:
    // facePaths: { -Z, +Z, -X, +X, -Y, +Y }  (back, front, left, right, bottom, top)
    Cube(glm::vec3 position, float size, const char* facePaths[6]);
    ~Cube() override;

    void draw(const ShaderProgram& shader) override;

    glm::vec3 position;
    float     size;

private:
    unsigned int textures[6];
};