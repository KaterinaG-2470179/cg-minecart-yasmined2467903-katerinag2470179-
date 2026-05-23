#pragma once

#include "FBO.h"
#include "ShaderProgram.h"

enum class PostProcessMode
{
    None,
    Blur,
    Sobel,
    ChromaKey,
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

    ShaderProgram blurShader;
    ShaderProgram sobelShader;
    ShaderProgram passShader;
    ShaderProgram chromaKeyShader;

    unsigned int quadVAO, quadVBO;
    unsigned int frameTexture;
};