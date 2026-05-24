#include "PostProcessor.h"
#include "TextureUtil.h"
#include <glad/glad.h>

static const float s_quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

static unsigned int makeColorTex(unsigned int w, unsigned int h) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

void PostProcessor::setupSceneFBO(unsigned int w, unsigned int h) {
    if (sceneFbo_id) { glDeleteFramebuffers(1, &sceneFbo_id); }
    if (sceneTexture) { glDeleteTextures(1, &sceneTexture); }
    if (brightTexture) { glDeleteTextures(1, &brightTexture); }
    if (depthRBO) { glDeleteRenderbuffers(1, &depthRBO); }

    glGenFramebuffers(1, &sceneFbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo_id);

    sceneTexture  = makeColorTex(w, h);
    brightTexture = makeColorTex(w, h);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture,  0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightTexture, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PostProcessor::PostProcessor(unsigned int width, unsigned int height)
    : width(width), height(height),

    blurFbo(width, height, false, true),
    blurShader("shaders/screen.vs", "shaders/blur.fs"),
    sobelShader("shaders/screen.vs", "shaders/sobel.fs"),
    passShader("shaders/screen.vs", "shaders/passthrough.fs"),
    chromaKeyShader("shaders/screen.vs", "shaders/chroma_key.fs"),
    bloomShader("shaders/screen.vs", "shaders/bloom.fs")
{
    setupSceneFBO(width, height);

    blurShader.use(); blurShader.setInt("screenTexture", 0);
    sobelShader.use(); sobelShader.setInt("screenTexture", 0);

    bloomShader.use();
    bloomShader.setInt("screenTexture", 0);
    bloomShader.setInt("bloomBlur", 1);
    bloomShader.setFloat("bloomStrength", 0.9f);

    frameTexture = loadTexture("resources/textures/pumpkin.png");
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int texW, texH;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &texW);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texH);

    chromaKeyShader.use();
    chromaKeyShader.setInt("screenTexture", 0);
    chromaKeyShader.setInt("sceneTexture", 1);
    chromaKeyShader.setFloat("threshold", 0.3f);
    chromaKeyShader.setFloat("softness", 0.1f);
    chromaKeyShader.setFloat("aspectRatio", (float)width / (float)height);
    chromaKeyShader.setFloat("imageAspect", (float)texW  / (float)texH);

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

void PostProcessor::setMode(PostProcessMode newMode) { mode = newMode; }

void PostProcessor::resize(unsigned int w, unsigned int h) {
    width = w; height = h;
    setupSceneFBO(w, h);
    blurFbo = FBO(w, h, false);
}

void PostProcessor::beginScene() {
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFbo_id);
    glEnable(GL_DEPTH_TEST);

    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if (nightMode)
        glClearColor(0.090f, 0.0f, 0.259f, 1.0f);
    else
        glClearColor(0.576f, 0.824f, 0.929f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);
}

void PostProcessor::renderQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::render() {
    glDisable(GL_DEPTH_TEST);

    if (mode == PostProcessMode::Bloom) {
        blurFbo.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        blurShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brightTexture);
        renderQuad();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        bloomShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, blurFbo.getTexture());
        renderQuad();
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (mode == PostProcessMode::ChromaKey) {
        chromaKeyShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);
        chromaKeyShader.setInt("screenTexture", 0);
        chromaKeyShader.setInt("sceneTexture", 1);
        renderQuad();
        return;
    }

    switch (mode) {
        case PostProcessMode::Blur: blurShader.use(); break;
        case PostProcessMode::Sobel: sobelShader.use(); break;
        default: passShader.use(); break;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    renderQuad();
}

void PostProcessor::setNightMode(bool night) {
    nightMode = night;
}