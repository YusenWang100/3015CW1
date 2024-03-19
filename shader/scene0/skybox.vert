#version 430 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform UBOCamera {
    mat4 V;
    mat4 P;
    vec3 viewPosition;
};

void main() {
    TexCoords = aPos;
    vec4 pos = P * vec4(mat3(V) * aPos, 1.0);
    gl_Position = pos.xyww;
}