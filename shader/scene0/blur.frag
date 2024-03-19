#version 410
out vec4 fragColor;

in vec2 vsTexcoord;

uniform sampler2D tex;

uniform bool horizontal;

const float c[13] =
    float[13](0.0010, 0.0051, 0.0197, 0.0565, 0.1199, 0.1883, 0.2189, 0.1883, 0.1199, 0.0565, 0.0197, 0.0051, 0.0010);

void main() {
    vec2 texSize = textureSize(tex, 0);
    vec3 col = vec3(0);
    int r = 6;
    if (horizontal) {
        for (int i = -r; i <= r; ++i) {
            col += texture(tex, vsTexcoord + vec2(i, 0) / texSize).rgb * c[i + r];
        }
    } else {
        for (int i = -r; i <= r; ++i) {
            col += texture(tex, vsTexcoord + vec2(0, i) / texSize).rgb * c[i + r];
        }
    }
    fragColor = vec4(col, 1);
}