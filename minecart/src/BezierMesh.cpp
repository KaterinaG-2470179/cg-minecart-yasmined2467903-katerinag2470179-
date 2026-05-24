#include "BezierMesh.h"
#include <glad/glad.h>

BezierMesh::BezierMesh(Bezier& bezier)
    : m_bezier(bezier)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    update();
}

void BezierMesh::update() {
    const auto& pts = m_bezier.GetPoints();
    m_count = pts.size();

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        pts.size() * sizeof(glm::vec3),
        pts.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void BezierMesh::draw(const ShaderProgram& shader) {
    shader.setMat4("model", glm::mat4(1.0f));

    glLineWidth(3.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)m_count);
    glBindVertexArray(0);
    glLineWidth(1.0f);
}