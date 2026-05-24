#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform float bloomStrength;

void main() {
    vec3 scene = texture(screenTexture, TexCoords).rgb;
    vec3 bloom = texture(bloomBlur, TexCoords).rgb;
    FragColor = vec4(scene + bloom * bloomStrength, 1.0);
}