#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
 
class PointLight {
    public:
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    
        float constant = 1.0f;
        float linear = 0.35f;
        float quadratic = 0.44f;
    
        PointLight(glm::vec3 position,
                glm::vec3 color = glm::vec3(1.0f, 0.65f, 0.1f),
                float intensity = 2.0f);
    
        void upload(const ShaderProgram& shader, int index, float intensityMult = 1.0f) const;
};
 
