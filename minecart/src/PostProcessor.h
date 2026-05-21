#pragma once

#include "FBO.h"
#include "ShaderProgram.h"

enum class PostProcessMode
{
    None,
    Blur,
    Sobel
};

class PostProcessor
{
public:
    PostProcessor(unsigned int width, unsigned int height);

    // Call at the start of each frame -> start drawing into the scene FBO
    void beginScene();
    void render();

    void setMode(PostProcessMode newMode);
    void resize(unsigned int width, unsigned int height);

private:
    PostProcessMode mode = PostProcessMode::None;

    FBO sceneFbo;   // pass 1: scene renders
    FBO blurFbo;    // pass 2: blurred

    ShaderProgram blurShader;
    ShaderProgram sobelShader;
    ShaderProgram passShader;

    unsigned int quadVAO, quadVBO;
};