#pragma once

#include "FBO.h"
#include "ShaderProgram.h"

enum class PostProcessMode {
    None,
    Blur,
    Sobel,
    ChromaKey,
    Bloom
};

class PostProcessor {
    public:
        PostProcessor(unsigned int width, unsigned int height);

        // call at the start of each frame -> start drawing into the scene FBO
        void beginScene();
        void render();
        void setNightMode(bool night);
        void setMode(PostProcessMode newMode);
        void resize(unsigned int width, unsigned int height);

    private:
        void renderQuad();
        void setupSceneFBO(unsigned int w, unsigned int h);

        unsigned int width, height;
        bool nightMode;

        unsigned int sceneFbo_id = 0;
        unsigned int sceneTexture = 0;
        unsigned int brightTexture = 0;
        unsigned int depthRBO = 0;

        FBO blurFbo;

        ShaderProgram blurShader;
        ShaderProgram sobelShader;
        ShaderProgram passShader;
        ShaderProgram chromaKeyShader;
        ShaderProgram bloomShader;

        unsigned int frameTexture = 0;
        unsigned int quadVAO = 0, quadVBO = 0;

        PostProcessMode mode = PostProcessMode::None;
};