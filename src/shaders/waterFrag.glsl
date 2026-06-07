#version 330

in vec2 TexCoords;

uniform sampler2D sampler;

out vec4 FragColor;

void main()
{
	FragColor = texture(sampler, TexCoords);
}
