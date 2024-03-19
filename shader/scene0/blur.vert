#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec2 inTexcoord;

out vec2 vsTexcoord;

void main() {
    vsTexcoord = inTexcoord;
    gl_Position = vec4(inPosition, 1);
}