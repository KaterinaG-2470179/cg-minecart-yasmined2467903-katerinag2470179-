#pragma once

#include "Mesh.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ShaderProgram.h"

class Torch : public Mesh {
    public:
        explicit Torch(glm::vec3 position = glm::vec3(0.0f), float scale = 0.5f);
        ~Torch() override;

        void draw(const ShaderProgram& shader) override;

        glm::vec3 position;
        float scale;

    private:
        unsigned int textures[3] = {};
};