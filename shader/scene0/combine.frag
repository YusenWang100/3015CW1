#version 410
out vec4 fragColor;

in vec2 vsTexcoord;

uniform sampler2D tex;
uniform sampler2D texHigh;

void main() {
    // fragColor = vec4(texture(texHigh, vsTexcoord).rgb, 1);
    fragColor = vec4(texture(texHigh, vsTexcoord).rgb + texture(tex, vsTexcoord).rgb, 1);
}