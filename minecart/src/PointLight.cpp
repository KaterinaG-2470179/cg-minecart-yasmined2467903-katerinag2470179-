#include "PointLight.h"
#include <cstdio>

PointLight::PointLight(glm::vec3 pos, glm::vec3 col, float intensity)
    : position(pos), color(col), intensity(intensity)
{}

void PointLight::upload(const ShaderProgram& shader, int index, float intensityMult) const
{
    char buf[64];
    auto u = [&](const char* field) -> const char* {
        snprintf(buf, sizeof(buf), "pointLights[%d].%s", index, field);
        return buf;
    };

    glm::vec3 c = color * intensity * intensityMult;

    shader.setVec3 (u("position"), position.x, position.y, position.z);
    shader.setVec3 (u("ambient"), c.x*0.15f, c.y*0.15f, c.z*0.15f);
    shader.setVec3 (u("diffuse"), c.x, c.y, c.z);
    shader.setVec3 (u("specular"), c.x*0.5f, c.y*0.5f, c.z*0.5f);
    shader.setFloat(u("constant"), constant);
    shader.setFloat(u("linear"), linear);
    shader.setFloat(u("quadratic"), quadratic);
}