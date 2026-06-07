#version 330 core 

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 PV;

smooth out vec2 fTexCoords;

void main()
{
    fTexCoords = texCoords;
    gl_Position = PV * vec4(position, 1.0);
}
