#version 430 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 fragColor2;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
    fragColor2 = vec4(0);
}