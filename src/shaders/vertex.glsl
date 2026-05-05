#version 330 core 

in vec3 position;
in vec3 normal;
in vec2 texCoords;

uniform mat4 PV;
uniform mat4 model;

smooth out vec2 fTexCoords;
smooth out vec3 fNormal;
smooth out vec3 FragPos;

void main()
{
	FragPos = vec3(model * vec4(position, 1.0));
    fTexCoords = texCoords;
    fNormal = mat3(transpose(inverse(model))) * normal;
    gl_Position = PV * vec4(FragPos, 1.0);
}
