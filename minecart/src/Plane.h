#pragma once

#include "Mesh.h"

class Plane : public Mesh {
    public:
        Plane();
        ~Plane() override = default;

        void draw(const ShaderProgram& shader) override;

    private:
        unsigned int texture;
        int vertCount = 0;
};