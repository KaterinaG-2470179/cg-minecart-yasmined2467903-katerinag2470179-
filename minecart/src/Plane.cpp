#include "Plane.h"

#include "TextureUtil.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

static const float RADIUS = 20.0f;
static const int   SEGMENTS = 64;

Plane::Plane() : texture(loadTexture("resources/textures/minefloor.png")) {
    std::vector<float> verts;

    float angleStep = 2.0f * 3.14159265f / SEGMENTS;

    for (int i = 0; i < SEGMENTS; i++) {
        float a0 = i * angleStep;
        float a1 = (i + 1) * angleStep;

        float x0 = cosf(a0) * RADIUS, z0 = sinf(a0) * RADIUS;
        float x1 = cosf(a1) * RADIUS, z1 = sinf(a1) * RADIUS;

        auto push = [&](float x, float z) {
            verts.insert(verts.end(), {
                x, 0.0f, z,
                0.0f, 1.0f, 0.0f,
                (x / RADIUS * 0.5f + 0.5f),
                (z / RADIUS * 0.5f + 0.5f)
            });
        };

        push(0.0f, 0.0f);
        push(x0, z0);
        push(x1, z1);
    }

    vertCount = (int)verts.size() / 8;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
}

void Plane::draw(const ShaderProgram& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glDrawArrays(GL_TRIANGLES, 0, vertCount);
    glBindVertexArray(0);
}

