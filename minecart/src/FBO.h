#pragma once
#include <glad/glad.h>
#include <iostream>

class FBO
{
public:
    unsigned int fbo;
    unsigned int texture;

    FBO() : fbo(0), texture(0), rbo(0) {}
    FBO(unsigned int width, unsigned int height, bool withDepth = false);
    ~FBO();

    FBO(const FBO&)            = delete;
    FBO& operator=(const FBO&) = delete;

    FBO(FBO&& other) noexcept;
    FBO& operator=(FBO&& other) noexcept;

    void bind()   const;
    void unbind() const;
    unsigned int getTexture() const { return texture; }

private:
    unsigned int rbo = 0;
};