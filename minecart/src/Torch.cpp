#include "Torch.h"

#include "TextureUtil.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

static const char* s_facePaths[3] = {
    "resources/torch_side.png",
    "resources/torch_bottom.png",
    "resources/torch_top.png",
};

static const float s_vertices[] = {
    // Front (+Z) normal  0, 0, 1
    -0.25f, 0.0f, 0.25f,   0,0,1,  0,0,
     0.25f, 0.0f, 0.25f,   0,0,1,  1,0,
     0.25f, 3.0f, 0.25f,   0,0,1,  1,1,
    -0.25f, 0.0f, 0.25f,   0,0,1,  0,0,
     0.25f, 3.0f, 0.25f,   0,0,1,  1,1,
    -0.25f, 3.0f, 0.25f,   0,0,1,  0,1,
 
    // Back (-Z) normal  0, 0,-1
     0.25f, 0.0f, -0.25f,  0,0,-1, 0,0,
    -0.25f, 0.0f, -0.25f,  0,0,-1, 1,0,
    -0.25f, 3.0f, -0.25f,  0,0,-1, 1,1,
     0.25f, 0.0f, -0.25f,  0,0,-1, 0,0,
    -0.25f, 3.0f, -0.25f,  0,0,-1, 1,1,
     0.25f, 3.0f, -0.25f,  0,0,-1, 0,1,
 
    // Left (-X) normal -1, 0, 0
    -0.25f, 0.0f,  0.25f, -1,0,0,  1,0,
    -0.25f, 0.0f, -0.25f, -1,0,0,  0,0,
    -0.25f, 3.0f, -0.25f, -1,0,0,  0,1,
    -0.25f, 0.0f,  0.25f, -1,0,0,  1,0,
    -0.25f, 3.0f, -0.25f, -1,0,0,  0,1,
    -0.25f, 3.0f,  0.25f, -1,0,0,  1,1,
 
    // Right (+X) normal  1, 0, 0
     0.25f, 0.0f, -0.25f,  1,0,0,  1,0,
     0.25f, 0.0f,  0.25f,  1,0,0,  0,0,
     0.25f, 3.0f,  0.25f,  1,0,0,  0,1,
     0.25f, 0.0f, -0.25f,  1,0,0,  1,0,
     0.25f, 3.0f,  0.25f,  1,0,0,  0,1,
     0.25f, 3.0f, -0.25f,  1,0,0,  1,1,
 
    // Bottom (-Y) normal  0,-1, 0
    -0.25f, 0.0f, -0.25f,  0,-1,0, 0,1,
     0.25f, 0.0f, -0.25f,  0,-1,0, 1,1,
     0.25f, 0.0f,  0.25f,  0,-1,0, 1,0,
    -0.25f, 0.0f, -0.25f,  0,-1,0, 0,1,
     0.25f, 0.0f,  0.25f,  0,-1,0, 1,0,
    -0.25f, 0.0f,  0.25f,  0,-1,0, 0,0,
 
    // Top (+Y) normal  0, 1, 0
    -0.25f, 3.0f,  0.25f,  0,1,0,  0,0,
     0.25f, 3.0f,  0.25f,  0,1,0,  1,0,
     0.25f, 3.0f, -0.25f,  0,1,0,  1,1,
    -0.25f, 3.0f,  0.25f,  0,1,0,  0,0,
     0.25f, 3.0f, -0.25f,  0,1,0,  1,1,
    -0.25f, 3.0f, -0.25f,  0,1,0,  0,1,
};

Torch::Torch(glm::vec3 pos) : position(pos) {
    for (int i = 0; i < 3; i++)
        textures[i] = loadTexture(s_facePaths[i]);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    glBindVertexArray(0);
}

Torch::~Torch() {
    for (int i = 0; i < 3; i++)
        glDeleteTextures(1, &textures[i]);
}

void Torch::draw(const ShaderProgram& shader) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);

    // 4 sides (idx 0) bottom (1) and top (2)
    static const int faceTexIndex[6] = { 0, 0, 0, 0, 1, 2 };
    for (int i = 0; i < 6; i++) {
        glBindTexture(GL_TEXTURE_2D, textures[faceTexIndex[i]]);
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }

    glBindVertexArray(0);
}