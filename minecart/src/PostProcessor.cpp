#include "PostProcessor.h"
#include "Cube.h"  // load texture

#include <glad/glad.h>

static const float s_quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

PostProcessor::PostProcessor(unsigned int width, unsigned int height)
    : sceneFbo(width, height, true), // true: needs depth
      blurShader ("shaders/screen.vs", "shaders/blur.fs"),
      sobelShader("shaders/screen.vs", "shaders/sobel.fs"),
      passShader("shaders/screen.vs", "shaders/passthrough.fs"),
      chromaKeyShader("shaders/screen.vs", "shaders/chroma_key.fs")
{
    blurShader.use();
    blurShader.setInt("screenTexture", 0);

    sobelShader.use();
    sobelShader.setInt("screenTexture", 0);

    //frameTexture = loadTexture("resources/pumpkin.png");
    frameTexture = loadTexture("resources/frame.jpg");

    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int texW, texH;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &texW);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);
    float imageAspect = (float)texW / (float)texH;

    chromaKeyShader.use();
    chromaKeyShader.setInt("screenTexture", 0);
    chromaKeyShader.setInt("sceneTexture", 1);
    chromaKeyShader.setFloat("threshold", 0.3f);
    chromaKeyShader.setFloat("softness",  0.1f);
    chromaKeyShader.setFloat("aspectRatio", (float)width / (float)height);
    chromaKeyShader.setFloat("imageAspect", imageAspect);

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_quadVertices), s_quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void PostProcessor::setMode(PostProcessMode newMode)
{
    mode = newMode;
}

void PostProcessor::resize(unsigned int width, unsigned int height)
{
    sceneFbo = FBO(width, height, true);
}

void PostProcessor::beginScene()
{
    sceneFbo.bind();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::render()
{
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (mode == PostProcessMode::ChromaKey) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        chromaKeyShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sceneFbo.getTexture());
        chromaKeyShader.setInt("screenTexture", 0);
        chromaKeyShader.setInt("sceneTexture", 1);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    } else {
        switch(mode) {
            case PostProcessMode::Blur:   blurShader.use();  break;
            case PostProcessMode::Sobel:  sobelShader.use(); break;
            default:                      passShader.use();  break;
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneFbo.getTexture());
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
}