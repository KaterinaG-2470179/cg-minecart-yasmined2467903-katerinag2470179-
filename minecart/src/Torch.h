#pragma once

#include "Mesh.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "ShaderProgram.h"

class Torch : public Mesh {
    public:
        explicit Torch(glm::vec3 position = glm::vec3(0.0f));
        ~Torch() override;

        void draw(const ShaderProgram& shader) override;

        glm::vec3 position;

    private:
        unsigned int textures[3] = {};
};