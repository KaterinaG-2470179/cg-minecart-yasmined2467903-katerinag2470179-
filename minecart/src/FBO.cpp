#include "FBO.h"

FBO::FBO(unsigned int width, unsigned int height, bool withDepth)
    : fbo(0), texture(0), rbo(0)
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    if (withDepth) {
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO()
{
    if (rbo) glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}

FBO::FBO(FBO&& other) noexcept
    : fbo(other.fbo), texture(other.texture), rbo(other.rbo)
{
    other.fbo     = 0;
    other.texture = 0;
    other.rbo     = 0;
}

FBO& FBO::operator=(FBO&& other) noexcept
{
    if (this != &other) {
        if (rbo)     glDeleteRenderbuffers(1, &rbo);
        if (texture) glDeleteTextures(1, &texture);
        if (fbo)     glDeleteFramebuffers(1, &fbo);

        fbo     = other.fbo;
        texture = other.texture;
        rbo     = other.rbo;

        other.fbo     = 0;
        other.texture = 0;
        other.rbo     = 0;
    }
    return *this;
}

void FBO::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void FBO::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
