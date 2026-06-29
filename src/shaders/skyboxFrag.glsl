#version 450 core

in vec3 texCoords;
out vec4 color;

uniform samplerCube sampler;

void main()
{
    color = texture(sampler, texCoords);
}
