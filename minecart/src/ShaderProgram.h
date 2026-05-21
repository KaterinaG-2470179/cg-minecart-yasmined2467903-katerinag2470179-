#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
public:
    unsigned int ID;

    ShaderProgram(const char* vertPath, const char* fragPath);
    ~ShaderProgram();

    void use() const;

    void setInt  (const char* name, int value)               const;
    void setMat4 (const char* name, const glm::mat4& value)  const;

private:
    static std::string readFile(const char* path);
    static unsigned int compileShader(GLenum type, const char* src);
};