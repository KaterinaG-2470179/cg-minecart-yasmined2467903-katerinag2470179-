#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D screenTexture;
uniform float threshold;
uniform float softness;
uniform float aspectRatio;
uniform float imageAspect;

vec3 rgb2ycbcr(vec3 c) {
    return vec3(
        0.299 * c.r + 0.587 * c.g + 0.114 * c.b,
        -0.168736 * c.r - 0.331264 * c.g + 0.5 * c.b + 0.5,
        0.5 * c.r - 0.418688 * c.g - 0.081312 * c.b + 0.5
    );
}

void main() {
    vec2 uv = TexCoords;
    uv.x = (uv.x - 0.5) * aspectRatio + 0.5;
    uv.y = (uv.y - 0.5) * imageAspect + 0.5;

    vec4 frameColor = texture(screenTexture, uv);
    vec4 sceneColor = texture(sceneTexture, TexCoords);

    vec3 ycbcr = rgb2ycbcr(frameColor.rgb);
    float cb = ycbcr.y;

    float alpha = smoothstep(threshold, threshold + softness, cb);
    FragColor = mix(sceneColor, frameColor, alpha);
}