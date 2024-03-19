#version 410

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexcoord;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in vec3 inBitangnet;

out VS_OUT
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
    vec3 bitangent;
}vs_out;

uniform UBOCamera
{
    mat4 V;
    mat4 P;
    vec3 viewPosition;
    float fogFactor;
    vec3 fogColor;
};
uniform UBOTransform
{
    mat4 M;
};

void main() {
    vs_out.position = (M * vec4(inPosition, 1.0)).xyz;
    vs_out.normal = (transpose(inverse(M)) * vec4(inNormal, 0.0)).xyz;
    vs_out.texcoord = inTexcoord;

    vs_out.tangent = normalize(transpose(inverse(M)) * vec4(inTangent, 0)).xyz;
    vs_out.bitangent = normalize(cross(vs_out.normal, vs_out.tangent));

    gl_Position = P * V * vec4(vs_out.position, 1.0);
}
