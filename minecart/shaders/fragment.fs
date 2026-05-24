#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

#define MAX_POINT_LIGHTS 16
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int numPointLights;

uniform vec3 viewPos;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo) {
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3  reflDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflDir), 0.0), 32.0);

    vec3 ambient = light.ambient  * albedo;
    vec3 diffuse = light.diffuse  * diff * albedo;
    vec3 specular = light.specular * spec * albedo;
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo)
{
    vec3  lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3  reflDir  = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflDir), 0.0), 32.0);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = light.ambient * albedo * attenuation;
    vec3 diffuse = light.diffuse * diff * albedo * attenuation;
    vec3 specular = light.specular * spec * albedo * attenuation;
    return ambient + diffuse + specular;
}

void main() {
    vec3 albedo = texture(tex, TexCoord).rgb;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = calcDirLight(dirLight, norm, viewDir, albedo);

    for (int i = 0; i < numPointLights && i < MAX_POINT_LIGHTS; i++)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir, albedo);

    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2, 0.7, 0.07));
    BrightColor = brightness > 1.0 ? vec4(result, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}