#pragma once

#include "Mesh.h"

class Plane : public Mesh
{
public:
    // texture: a single GL texture id (load it with loadTexture() first)
    Plane(unsigned int texture);
    ~Plane() override = default;

    void draw(const ShaderProgram& shader) override;

private:
    unsigned int texture;
};