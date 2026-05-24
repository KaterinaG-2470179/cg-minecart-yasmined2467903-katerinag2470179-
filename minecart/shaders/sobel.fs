#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 700.0;

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), vec2(0.0, offset), vec2(offset, offset),
        vec2(-offset, 0.0), vec2(0.0, 0.0), vec2(offset, 0.0),
        vec2(-offset, -offset), vec2(0.0, -offset), vec2(offset, -offset)
    );

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++) {
        vec3 col = vec3(texture(screenTexture, TexCoords + offsets[i]));
        float grey = dot(col, vec3(0.3, 0.6, 0.1));
        sampleTex[i] = vec3(grey);
    }

    // horizontal and vertical sobel kernels
    float Kx[9] = float[](-1, 0, 1,
                          -2, 0, 2,
                          -1, 0, 1);

    float Ky[9] = float[]( 1,  2,  1,
                           0,  0,  0,
                          -1, -2, -1);

    vec3 Gx = vec3(0.0);
    vec3 Gy = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        Gx += sampleTex[i] * Kx[i];
        Gy += sampleTex[i] * Ky[i];
    }

    vec3 edges = sqrt(Gx * Gx + Gy * Gy);

    FragColor = vec4(edges, 1.0);
}

